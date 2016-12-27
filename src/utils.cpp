#include <utils.h>


std::string ffloat(float source_num, int digits) {
	/* Format a float as a string */
	bool _sign = source_num < 0;
	source_num = std::abs(source_num);
	float _rem = fmod(source_num, 1.);
	int _int = (int) source_num;
	std::ostringstream _rsstr;
	_rsstr << (int)(_rem * pow(10, digits));
	std::string _rs = _rsstr.str();
	while (_rs.length() < digits) {
		std::ostringstream _new;
		_new << '0' << _rs;
		_rs = _new.str();
	}
	std::ostringstream _acc;
	_acc << ((_sign) ? '-' : ' ');
	_acc << _int << '.' << _rs;
	return _acc.str();
}


