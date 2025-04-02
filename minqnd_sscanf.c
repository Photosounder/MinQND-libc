int vsscanf(const char *s, const char *fmt, va_list arg)
{
	int match_count = 0, ret_eof = 1;
	size_t f_pos, s_pos = 0;

	for (f_pos=0; ; f_pos++)
	{
		if (fmt[f_pos] == '%')
		{
			f_pos++;

			int match = 0;

			// Read suppression flags
			int flag_suppr = 0;
			while (fmt[f_pos]=='*')
			{
				flag_suppr = 1;
				f_pos++;
			}

			// Read field width
			int field_width = INT_MAX;
			if (fmt[f_pos] >= '1' && fmt[f_pos] <= '9')
			{
				field_width = fmt[f_pos] - '0';
				f_pos++;
				while (isdigit(fmt[f_pos]))
				{
					field_width = field_width*10 + fmt[f_pos] - '0';
					f_pos++;
				}
			}

			// Read length modifiers
			char len_mod = 0, len_mod_doubled = 0;
			if (fmt[f_pos]=='h' || fmt[f_pos]=='l' || fmt[f_pos]=='j' || fmt[f_pos]=='z' || fmt[f_pos]=='t' || fmt[f_pos]=='L')
			{
				len_mod = fmt[f_pos];
				if ((fmt[f_pos]=='h' || fmt[f_pos]=='l') && fmt[f_pos]==fmt[f_pos+1])
				{
					len_mod_doubled = 1;
					f_pos++;
				}
				f_pos++;
			}

			// Read conversion specifier
			char conv_spec = fmt[f_pos];
			int conv_is_int =   (conv_spec == 'd' || conv_spec == 'i' || conv_spec == 'o' || conv_spec == 'u' || conv_spec == 'x' || conv_spec == 'X');
			int conv_is_float = (conv_spec == 'g' || conv_spec == 'G' || conv_spec == 'f' || conv_spec == 'F' || conv_spec == 'e' || conv_spec == 'E');

			if (conv_spec == 'n')
			{
				int *n = va_arg(arg, int *);
				if (flag_suppr == 0)
					*n = s_pos;
			}

			// Parse %
			if (conv_spec == '%')
				if ('%' != s[s_pos++])
					goto eof_reached;

			// Parse char
			if (conv_spec == 'c')
			{
				if (flag_suppr == 0)
				{
					char *vc = va_arg(arg, char *);
					*vc = s[s_pos];
				}
				s_pos++;
				match = 1;
			}

			// Parse string
			if (conv_spec == 's' || conv_spec == '[')
			{
				char *vs;
				if (flag_suppr == 0)
					vs = va_arg(arg, char *);

				int is, scanset_start = f_pos + 1;
				for (is = 0; s[s_pos] && (!isspace(s[s_pos]) || conv_spec == '[') && field_width; is++, s_pos++, field_width--)
				{
					int i, copy = 0, neg = 0;

					if (conv_spec == 's')
						copy = 1;
					else
					{
						// Go through scanset and test char
						for (i = scanset_start; fmt[i] != ']'; i++)
						{
							unsigned int c0, c1;

							// Negation by [^...]
							if (fmt[i] == '^' && i == scanset_start)
							{
								neg = 1;
								i++;
							}

							// Range
							c0 = fmt[i];
							if (fmt[i+1] == '-')
							{
								i += 2;
								c1 = fmt[i];
							}
							else
								c1 = c0;

							// Test
							if (s[s_pos] >= c0 && s[s_pos] <= c1)
								copy = 1;
						}

						f_pos = i;
						copy ^= neg;
					}

					if (copy && flag_suppr == 0)
						vs[is] = s[s_pos];

					if (copy == 0)
						break;
				}

				if (flag_suppr == 0)
					vs[is] = '\0';

				match = 1;
			}

			// Handle pointer
			if (conv_spec == 'p')
			{
				// Turn %p into %zx
				len_mod = 'z';
				conv_spec = 'x';
			}

			// Read sign
			int neg = 0;
			if (conv_is_int || conv_is_float)
				if (s[s_pos] == '-' || s[s_pos] == '+')
					neg = (s[s_pos++] == '-');

			// Skip 0x and handle hexadecimal for %i
			if (conv_spec == 'i' || conv_spec == 'x' || conv_spec == 'X')
				if (s[s_pos] == '0' && (s[s_pos+1] == 'x' || s[s_pos+1] == 'X') && isxdigit(s[s_pos+2]))
				{
					s_pos += 2;
					conv_spec = 'x';
				}

			// Read decimal
			intmax_t vi = 0;
			if (conv_spec == 'd' || conv_spec == 'i' || conv_spec == 'u')
			{
				// Check validity
				if (!isdigit(s[s_pos]))
					goto eof_reached;
				match = 1;

				// Read and add up digits
				do
				{
					int d = s[s_pos++] - '0';
					vi = vi*10 + d;
				}
				while (isdigit(s[s_pos]));
			}

			// Read hexadecimal
			if (conv_spec == 'x' || conv_spec == 'X')
			{
				// Check validity
				if (!isxdigit(s[s_pos]))
					goto eof_reached;
				match = 1;

				// Read and add up digits
				do
				{
					int d = s[s_pos++];
					if (isdigit(d))
						d -= '0';
					else if (d <= 'F')
						d -= 'A' - 10;
					else
						d -= 'a' - 10;
					vi = vi*16 + d;
				}
				while (isxdigit(s[s_pos]));
			}

			// Read float
			double vf = 0.;
			if (conv_is_float)
			{
				int dot_exp = 1;

				// Check NAN
				if (tolower(s[s_pos]) == 'n' && tolower(s[s_pos+1]) == 'a' && tolower(s[s_pos+2]) == 'n')
				{
					match = 1;
					vf = NAN;
					s_pos += 3;
					goto float_conv_end;
				}

				// Check INF
				if (tolower(s[s_pos]) == 'i' && tolower(s[s_pos+1]) == 'n' && tolower(s[s_pos+2]) == 'f')
				{
					match = 1;
					vf = INFINITY;
					s_pos += 3;
					goto float_conv_end;
				}

				// Check validity
				if (!isdigit(s[s_pos]) && s[s_pos] != '.')
					goto eof_reached;
				match = 1;

				// Read and add up digits and track the dot
				do
				{
					int d = s[s_pos++];
					if (isdigit(d))
					{
						vf = vf*10. + (double) (d-'0');
						if (dot_exp < 1)
							dot_exp--;
					}
					else
						dot_exp = 0;
				}
				while (isdigit(s[s_pos]) || s[s_pos] == '.');

				if (dot_exp == 1)
					dot_exp = 0;

				// Handle exponent
				if (s[s_pos] == 'e')
				{
					int e10 = 0, e_neg = 0;
					s_pos++;

					// Exponent sign
					if (s[s_pos] == '-' || s[s_pos] == '+')
						e_neg = (s[s_pos++] == '-');

					if (isdigit(s[s_pos]))
					{
						// Exponent digits
						do
						{
							int d = s[s_pos++] - '0';
							e10 = e10*10 + d;
						}
						while (isdigit(s[s_pos]));

						// Apply sign
						if (e_neg)
							e10 = -e10;

						dot_exp += e10;
					}
				}

				// Apply exponent
				vf = apply_power_of_10(vf, dot_exp);
			}
float_conv_end:

			// Apply sign
			if (neg)
			{
				if (conv_is_int)
					vi = -vi;
				else
					vf = -vf;
			}

			// Store integer
			if (conv_is_int && flag_suppr == 0)
			{
				switch (len_mod)
				{
						case 'h':  if (len_mod_doubled)
							   { signed char   *pv = va_arg(arg, signed char *);	*pv = vi; } else
							   { short int	   *pv = va_arg(arg, short int *);	*pv = vi; }
					break;	case 'l':  if (len_mod_doubled)
							   { long long int *pv = va_arg(arg, long long int *);	*pv = vi; } else
							   { long int	   *pv = va_arg(arg, long int *);	*pv = vi; }
					break;	case 'j':  { intmax_t	   *pv = va_arg(arg, intmax_t *);	*pv = vi; }
					break;	case 'z':  { size_t	   *pv = va_arg(arg, size_t *);		*pv = vi; }
					break;	case 't':  { ptrdiff_t	   *pv = va_arg(arg, ptrdiff_t *);	*pv = vi; }
					break;	default:   { int	   *pv = va_arg(arg, int *);		*pv = vi; }
				}
			}

			// Store float
			if (conv_is_float && flag_suppr == 0)
			{
				switch (len_mod)
				{
						case 'L':	// long double falls back to double
						case 'l':  { double *pf = va_arg(arg, double *); *pf = vf; }
					break;	default:   { float  *pf = va_arg(arg, float *);  *pf = vf; }
				}
			}

			if (match)
			{
				ret_eof = 0;
				if (flag_suppr == 0)
					match_count++;
			}

			continue;

		}

		// Whitespace skipping
		if (isspace(fmt[f_pos]))
		{
			while (isspace(s[s_pos]))
				s_pos++;
			continue;
		}

		// Normal char matching
		if (fmt[f_pos] != s[s_pos++])
			goto eof_reached;

		if (fmt[f_pos] == '\0')
			break;
	}

	return match_count;
eof_reached:
	return match_count || ret_eof == 0 ? match_count : EOF;
}

int sscanf(const char *s, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vsscanf(s, format, args);
	va_end(args);
	return ret;
}
