/*
 * Copyright (C) 2014 Bastian Bloessl <bloessl@ccs-labs.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef INCLUDED_RDS_PARSER_IMPL_H
#define INCLUDED_RDS_PARSER_IMPL_H

#include ../rds/parser.h
#include <gnuradio/thread/thread.h>
#include rdsdecoder.h

namespace gr {
namespace rds {

class parser_impl : public parser
{
public:
	parser_impl(bool log, bool debug, unsigned char pty_locale);

private:
	~parser_impl();
    rdsdecoder_ptr *decoder;
	bool           debug;
	bool           log;
	unsigned char  pty_locale;
	gr::thread::mutex d_mutex;
};

} /* namespace rds */
} /* namespace gr */

#endif /* INCLUDED_RDS_PARSER_IMPL_H */

