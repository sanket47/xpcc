// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__COMMUNICATION_HPP
#define	XPCC__COMMUNICATION_HPP

#include "backend/backend_interface.hpp"
#include "postman/postman.hpp"
#include "communication_list.hpp"

namespace xpcc
{
	typedef Header ResponseHandle;
	
	/**
	 * \todo	Documentation
	 * \ingroup	communication
	 */
	class Communication
	{
	public:
		Communication(
				BackendInterface *backend,
				Postman* postman);
		
		void
		update();
		
		// [proposition -> dergraaf]: Make these methods only available in the correct
		// circumstances (action call). Perhaps move them methods to the ResponseHandle
		// class?
		void
		sendResponse(const ResponseHandle& handle);

		template<typename T>
		void
		sendResponse(const ResponseHandle& handle, const T& data);
		
		template<typename T>
		void
		sendNegativeResponse(const ResponseHandle& handle, const T& data);
		
		void
		sendNegativeResponse(const ResponseHandle& handle);
		
	private:
		BackendInterface * const backend;
		Postman * postman;
		communicationList::List responseManager;
		
		void
		waitForAcknowledge(const Header &header, const SmartPointer& payload);
	
	private:
		friend class AbstractComponent;
	};
}

// -----------------------------------------------------------------------------
// IMPLEMENTATION
// -----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communication::sendResponse(const ResponseHandle& handle, const T& data)
{
	Header header(	Header::RESPONSE,
					false,
					handle.source,
					handle.destination,
					handle.packetIdentifier);

	SmartPointer payload(&data);

	this->responseManager.addResponse(header, payload);
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Communication::sendNegativeResponse(const ResponseHandle& handle, const T& data)
{
	Header header(	Header::NEGATIVE_RESPONSE,
					false,
					handle.source,
					handle.destination,
					handle.packetIdentifier);

	SmartPointer payload(&data);

	this->responseManager.addResponse(header, payload);
}

#endif // XPCC__COMMUNICATION_HPP