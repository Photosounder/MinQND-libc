double make_power_of_10(int p)
{
	double v = 1.;

	int neg = p < 0 ? 1 : 0;
	if (neg)
		p = -p;

	while (p >= 25) { v *= 1e25; p -= 25; }
	while (p >= 5)  { v *= 1e5;  p -= 5; }
	while (p >= 1)  { v *= 10.;  p -= 1; }

	if (neg)
		v = 1. / v;

	return v;
}

intmax_t make_power_of_10_int(int p)
{
	intmax_t v = 1;

	while (p >= 5)  { v *= 100000;  p -= 5; }
	while (p >= 1)  { v *= 10;     p -= 1; }

	return v;
}

int get_power_of_10_exponent(double v)
{
	v = fabs(v);

	// Guess power of 10 exponent from power of 2 exponent
	int dexp = ((double_as_int(v) >> 52) & 0x7FF) - 1023;
	int exponent = 1 + (dexp+1) * 4004/13301;

	// Adjust exponent
	double vd;
	do
	{
		exponent--;
		vd = v / make_power_of_10(exponent);
	}
	while (vd < 1.);
	return exponent;
}

double apply_power_of_10(double v, int e)
{
	if (e <= 0)
		return v / make_power_of_10(-e);
	else
		return v * make_power_of_10(e);
}

int vsnprintf(char *s, size_t s_len, const char *fmt, va_list arg)
{
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
				while (isdigit(fmt[f_pos]))
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
					while (isdigit(fmt[f_pos]))
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

			// Print %
			if (conv_spec == '%')
				if (s_pos++<s_len) s[s_pos-1] = '%';

			// Print string
			if (conv_spec == 's')
			{
				const char *string = va_arg(arg, const char *);

				size_t string_len = strlen(string);
				if (precision == -1)
					precision = string_len;		// "precision" = max bytes printed
				if (precision > string_len)
					precision = string_len;

				// Print padding
				if (field_width)
					for (int i=0; i < field_width - precision; i++)
						if (s_pos++<s_len) s[s_pos-1] = ' ';

				// Copy chars
				for (int i=0; i < precision && string[i]; i++)
					if (s_pos++<s_len) s[s_pos-1] = string[i];
			}

			// Handle pointer
			if (conv_spec == 'p')
			{
				// Turn %p into %zx
				len_mod = 'z';
				conv_spec = 'x';
			}

			// Read integer
			intmax_t vi;
			uintmax_t vu;
			if (conv_spec == 'd' || conv_spec == 'i')
			{
				switch (len_mod)
				{
						case 'l':  if (len_mod_doubled) vi = va_arg(arg, long long int); else vi = va_arg(arg, long int);
					break;	case 'j':  vi = va_arg(arg, intmax_t);
					break;	case 'z':  vi = va_arg(arg, size_t);
					break;	case 't':  vi = va_arg(arg, ptrdiff_t);
					break;	default:   vi = va_arg(arg, int);
				}
			}
			else if (conv_spec == 'o' || conv_spec == 'u' || conv_spec == 'x' || conv_spec == 'X')
			{
				switch (len_mod)
				{
						case 'l':  if (len_mod_doubled) vu = va_arg(arg, unsigned long long int); else vu = va_arg(arg, unsigned long int);
					break;	case 'j':  vu = va_arg(arg, uintmax_t);
					break;	case 'z':  vu = va_arg(arg, size_t);
					break;	case 't':  vu = va_arg(arg, ptrdiff_t);
					break;	default:   vu = va_arg(arg, unsigned int);
				}
			}

			// Print signed decimal
			if (conv_spec == 'd' || conv_spec == 'i')
			{
				// Print and remove sign
				if (vi < 0)
				{
					if (s_pos++<s_len) s[s_pos-1] = '-';
					vi = -vi;
				}

				if (vi == 0)
					if (s_pos++<s_len) s[s_pos-1] = '0';

				int e10 = get_power_of_10_exponent(vi);

				// Print digits
				for (; e10 >= 0; e10--)
				{
					intmax_t p = make_power_of_10_int(e10);
					int d = vi / p;
					if (s_pos++<s_len) s[s_pos-1] = '0' + d;
					vi -= d * p;
				}
			}

			// Print unsigned decimal
			if (conv_spec == 'u')
			{
				if (vu == 0)
					if (s_pos++<s_len) s[s_pos-1] = '0';

				int e10 = get_power_of_10_exponent(vu);

				// Print digits
				for (; e10 >= 0; e10--)
				{
					uintmax_t p = make_power_of_10_int(e10);
					unsigned int d = vu / p;
					if (s_pos++<s_len) s[s_pos-1] = '0' + d;
					vu -= d * p;
				}
			}

			// Print hexadecimal
			if (conv_spec == 'x' || conv_spec == 'X')
			{
				// Print 0x
				if (flag_alt && vu)
				{
					if (s_pos++<s_len) s[s_pos-1] = '0';
					if (s_pos++<s_len) s[s_pos-1] = conv_spec;
				}

				if (vu == 0)
					if (s_pos++<s_len) s[s_pos-1] = '0';

				int print_zeroes = 0;

				// Print digits
				for (int sh = sizeof(uintmax_t)*8-4; sh >= 0; sh-=4)
				{
					int d = (vu >> sh) & 0xF;

					if (d || print_zeroes)
					{
						if (d)
							print_zeroes = 1;

						if (d < 10)
						{
							if (s_pos++<s_len) s[s_pos-1] = '0' + d;
						}
						else
						{
							if (s_pos++<s_len) s[s_pos-1] = (conv_spec == 'X' ? 'A' : 'a') + d - 10;
						}
					}
				}
			}

			// Print double
			if (conv_spec == 'g' || conv_spec == 'G' || conv_spec == 'f' || conv_spec == 'F' || conv_spec == 'e' || conv_spec == 'E')
			{
				int capital = 32 * (conv_spec == 'G' || conv_spec == 'F' || conv_spec == 'E');
				conv_spec += capital;

				double v = va_arg(arg, double);

				// Print and remove sign
				if (v < 0.)
				{
					if (s_pos++<s_len) s[s_pos-1] = '-';
					v = -v;
				}

				// Zero
				if (v == 0.)
				{
					if (s_pos++<s_len) s[s_pos-1] = '0';
					goto end_double;
				}

				// NAN
				if (isnan(v))
				{
					if (s_pos++<s_len) s[s_pos-1] = 'n' - capital;
					if (s_pos++<s_len) s[s_pos-1] = 'a' - capital;
					if (s_pos++<s_len) s[s_pos-1] = 'n' - capital;
					goto end_double;
				}

				// INF
				if (!isfinite(v))
				{
					if (s_pos++<s_len) s[s_pos-1] = 'i' - capital;
					if (s_pos++<s_len) s[s_pos-1] = 'n' - capital;
					if (s_pos++<s_len) s[s_pos-1] = 'f' - capital;
					goto end_double;
				}

				if (precision == -1)
					precision = 6;

				// Round up and decide of levels
				int e10, start_lvl, end_lvl, dot_lvl;
				double vo = v;
				for (int i=0; i < 3; i++)
				{
					// Rounding
					if (i)
						v = vo + apply_power_of_10(5., end_lvl);

					// Decide of levels
					e10 = get_power_of_10_exponent(v);
					start_lvl = e10;

					if (conv_spec == 'g')
					{
						end_lvl = e10 - precision;
						dot_lvl = e10;

						if (e10 >= -4 && e10 < 0)
						{
							start_lvl = 0;
							dot_lvl = 0;
						}

						if (start_lvl > 0 && end_lvl < 0)
							dot_lvl = 0;
					}

					if (conv_spec == 'f')
					{
						dot_lvl = 0;
						end_lvl = -precision - 1;
					}

					if (conv_spec == 'e')
					{
						dot_lvl = start_lvl;
						end_lvl = start_lvl - precision - 1;
					}
				}

				// Print digits
				int last_pos = s_pos;
				for (int i=start_lvl; i > end_lvl; i--)
				{
					// Extract and subtract digit
					double d = floor(apply_power_of_10(v, -i));
					double dm = apply_power_of_10(d, i);
					if (dm > v)
					{
						d -= 1.;
						dm = apply_power_of_10(d, i);
					}
					v -= dm;

					// Print digit and check if it's 0
					if (s_pos++<s_len) s[s_pos-1] = '0' + (int) d;
					if (d > 0.)
						last_pos = s_pos;

					// Print dot
					if (i == dot_lvl)
					{
						last_pos = s_pos;
						if (s_pos++<s_len) s[s_pos-1] = '.';
					}
				}

				// Erase last zeroes
				if (conv_spec == 'g')
					s_pos = last_pos;

				// Print exponent
				if (dot_lvl || conv_spec == 'e')
				{
					if (s_pos++<s_len) s[s_pos-1] = 'e' - capital;
					if (s_pos++<s_len) s[s_pos-1] = dot_lvl < 0 ? '-' : '+';
					dot_lvl = abs(dot_lvl);
					if (dot_lvl >= 100)
					{
						if (s_pos++<s_len) s[s_pos-1] = '0' + dot_lvl / 100;
						dot_lvl -= (dot_lvl / 100) * 100;
					}
					if (s_pos++<s_len) s[s_pos-1] = '0' + dot_lvl / 10;
					dot_lvl -= (dot_lvl / 10) * 10;
					if (s_pos++<s_len) s[s_pos-1] = '0' + dot_lvl;
				}
			end_double:;
			}

			continue;
		}

		if (fmt[f_pos])
		{
			if (s_pos++<s_len) s[s_pos-1] = fmt[f_pos];
		}
		else
			break;
	}

	if (s_pos < s_len)
		s[s_pos] = '\0';
	else
		s[s_len-1] = '\0';
	return s_pos;
}

int vsprintf(char *s, const char *format, va_list args)
{
	return vsnprintf(s, SIZE_MAX, format, args);
}

int snprintf(char *s, size_t n, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vsnprintf(s, n, format, args);
	va_end(args);
	return ret;
}

int sprintf(char *s, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	int ret = vsnprintf(s, SIZE_MAX, format, args);
	va_end(args);
	return ret;
}
