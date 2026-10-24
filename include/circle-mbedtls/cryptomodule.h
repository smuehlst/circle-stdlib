//
// cryptomodule.h
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _circle_mbedtls_cryptomodule_h
#define _circle_mbedtls_cryptomodule_h

#include <psa/crypto.h>

namespace CircleMbedTLS {

class CCryptoModule		/// Initializes the PSA crypto subsystem (incl. its random generator)
{
public:
	/// \note CThreadingModule and CEntropyModule must have been constructed before
	CCryptoModule (void);
	~CCryptoModule (void);

	/// \return Result of psa_crypto_init() (PSA_SUCCESS on success)
	psa_status_t GetStatus (void) const;

private:
	static int m_nUseCount;
	static psa_status_t m_Status;
};

}

#endif
