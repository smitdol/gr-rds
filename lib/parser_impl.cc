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

#include "parser_impl.h"
#include "constants.h"
#include "tmc_events.h"
#include <gnuradio/io_signature.h>
#include <math.h>

using namespace gr::rds;

parser::sptr
parser::make(bool log, bool debug, unsigned char pty_locale) {
  return gnuradio::get_initial_sptr(new parser_impl(log, debug, pty_locale));
}

parser_impl::parser_impl(bool log, bool debug, unsigned char pty_locale)
	: gr::block ("gr_rds_parser",
			gr::io_signature::make (0, 0, 0),
			gr::io_signature::make (0, 0, 0)),
	log(log),
	debug(debug),
	pty_locale(pty_locale)
{
	message_port_register_in(pmt::mp("in"));
	set_msg_handler(pmt::mp("in"), boost::bind(&parser_impl::parse, this, _1));
	message_port_register_out(pmt::mp("out"));
    decoder_ptr = new decoder(log, debug, pty_locale, this->sendmessage);
	reset();
}

parser_impl::~parser_impl() {
    delete decoder_ptr;
    decoder_ptr = NULL;
}

void parser_impl::reset() {
	gr::thread::scoped_lock lock(d_mutex);
    decoder_ptr->reset();
}

/* type 0 = PI
 * type 1 = PS
 * type 2 = PTY
 * type 3 = flagstring: TP, TA, MuSp, MoSt, AH, CMP, stPTY
 * type 4 = RadioText 
 * type 5 = ClockTime
 * type 6 = Alternative Frequencies */
void parser_impl::send_message(long msgtype, std::string& msgtext) {
	pmt::pmt_t msg  = pmt::mp(msgtext);
	pmt::pmt_t type = pmt::from_long(msgtype);
	message_port_pub(pmt::mp("out"), pmt::make_tuple(type, msg));
}

void parser_impl::parse(pmt::pmt_t pdu) {
	if(!pmt::is_pair(pdu)) {
		dout << "wrong input message (not a PDU)" << std::endl;
		return;
	}

	//pmt::pmt_t meta = pmt::car(pdu);  // meta is currently not in use
	pmt::pmt_t vec = pmt::cdr(pdu);

	if(!pmt::is_blob(vec)) {
		dout << "input PDU message has wrong type (not u8)" << std::endl;
		return;
	}
	if(pmt::blob_length(vec) != 12) {  // 8 data + 4 offset chars (ABCD)
		dout << "input PDU message has wrong size ("
			<< pmt::blob_length(vec) << ")" << std::endl;
		return;
	}

	unsigned char *bytes = (unsigned char *)pmt::blob_data(vec);
	unsigned int group[4];
	group[0] = bytes[1] | (((unsigned int)(bytes[0])) << 8U);
	group[1] = bytes[3] | (((unsigned int)(bytes[2])) << 8U);
	group[2] = bytes[5] | (((unsigned int)(bytes[4])) << 8U);
	group[3] = bytes[7] | (((unsigned int)(bytes[6])) << 8U);
    
    decoder_ptr->parse(group);
}

