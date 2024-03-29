int vsscanf(const char *s, const char *fmt, va_list arg)
{
	int match_count = 0;
	size_t f_pos, s_pos = 0;

	for (f_pos=0; ; f_pos++)
	{
		if (fmt[f_pos] == '%')
		{
			f_pos++;

			// Read flags
			int flag_left_just=0, flag_plus=0, flag_space=0, flag_alt=0, flag_zero_pad=0;
			while (fmt[f_pos]=='-' || fmt[f_pos]=='+' || fmt[f_pos]==' ' || fmt[f_pos]=='#' || fmt[f_pos]=='0')
			{
				switch (fmt[f_pos])
				{
						case '-': flag_left_just = 1;
					break;	case '+': flag_plus = 1;
					break;	case ' ': flag_space = 1;
					break;	case '#': flag_alt = 1;
					break;	case '0': flag_zero_pad = 1;
				}
				f_pos++;
			}

			// Read field width
			int field_width = 0;
			if (fmt[f_pos] == '*')
			{
				field_width = va_arg(arg, int);
				f_pos++;
			}
			if (fmt[f_pos] >= '1' && fmt[f_pos] <= '9')
			{
				field_width = fmt[f_pos] - '0';
				f_pos++;
				while (fmt[f_pos] >= '0' && fmt[f_pos] <= '9')
				{
					field_width = field_width*10 + fmt[f_pos] - '0';
					f_pos++;
				}
			}

			// Read precision
			int precision = -1;
			if (fmt[f_pos] == '.')
			{
				precision = 0;
				f_pos++;
				if (fmt[f_pos] == '*')
				{
					precision = va_arg(arg, int);
					f_pos++;
				}
				if (fmt[f_pos] >= '1' && fmt[f_pos] <= '9')
				{
					precision = fmt[f_pos] - '0';
					f_pos++;
					while (fmt[f_pos] >= '0' && fmt[f_pos] <= '9')
					{
						precision = precision*10 + fmt[f_pos] - '0';
						f_pos++;
					}
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
				*n = s_pos;
			}

			// Parse %
			if (conv_spec == '%')
				if ('%' != s[s_pos++])
					return match_count ? match_count : EOF;

			// Parse char
			if (conv_spec == 'c')
			{
				char *vc = va_arg(arg, char *);
				*vc = s[s_pos++];
				match_count++;
			}

			// Parse string
			if (conv_spec == 's')
			{
				char *vs = va_arg(arg, char *);
				int is;
				for (is = 0; s[s_pos] && !isspace(s[s_pos]); is++, s_pos++)
					vs[is] = s[s_pos];
				vs[is] = '\0';
				match_count++;
			}

			// TODO Parse %[] scanset

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
					return match_count ? match_count : EOF;
				match_count++;

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
					return match_count ? match_count : EOF;
				match_count++;

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

				// Check validity
				if (!isdigit(s[s_pos]) && s[s_pos] != '.')
					return match_count ? match_count : EOF;
				match_count++;

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

			// Apply sign
			if (neg)
			{
				if (conv_is_int)
					vi = -vi;
				else
					vf = -vf;
			}

			// Store integer
			if (conv_is_int)
			{
				switch (len_mod)
				{
						case 'h':  if (len_mod_doubled)
							   { signed char   *pi = va_arg(arg, signed char *);	*pi = vi; } else
							   { short int	   *pi = va_arg(arg, short int *);	*pi = vi; }
					break;	case 'l':  if (len_mod_doubled)
							   { long long int *pi = va_arg(arg, long long int *);	*pi = vi; } else
							   { long int	   *pi = va_arg(arg, long int *);	*pi = vi; }
					break;	case 'j':  { intmax_t	   *pi = va_arg(arg, intmax_t *);	*pi = vi; }
					break;	case 'z':  { size_t	   *pi = va_arg(arg, size_t *);		*pi = vi; }
					break;	case 't':  { ptrdiff_t	   *pi = va_arg(arg, ptrdiff_t *);	*pi = vi; }
					break;	default:   { int	   *pi = va_arg(arg, int *);		*pi = vi; }
				}
			}

			// Store float
			if (conv_is_float)
			{
				switch (len_mod)
				{
						case 'l':  { double *pf = va_arg(arg, double *); *pf = vf; }
					break;	default:   { float  *pf = va_arg(arg, float *);  *pf = vf; }
				}
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
			return match_count ? match_count : EOF;

		if (fmt[f_pos] == '\0')
			break;
	}

	return match_count;
}

int sscanf(const char *s, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vsscanf(s, format, args);
	va_end(args);
	return ret;
}