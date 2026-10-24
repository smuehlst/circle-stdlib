//
// cryptomodule.cpp
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
#include <circle-mbedtls/cryptomodule.h>
#include <mbedtls/platform_time.h>
#include <circle/timer.h>
#include <assert.h>

using namespace CircleMbedTLS;

int CCryptoModule::m_nUseCount = 0;
psa_status_t CCryptoModule::m_Status = PSA_ERROR_BAD_STATE;

CCryptoModule::CCryptoModule (void)
{
	if (m_nUseCount++ == 0)
	{
		m_Status = psa_crypto_init ();
	}
}

CCryptoModule::~CCryptoModule (void)
{
	assert (m_nUseCount > 0);
	if (--m_nUseCount == 0)
	{
		mbedtls_psa_crypto_free ();

		m_Status = PSA_ERROR_BAD_STATE;
	}
}

psa_status_t CCryptoModule::GetStatus (void) const
{
	return m_Status;
}

// implements the MBEDTLS_PLATFORM_MS_TIME_ALT config option
mbedtls_ms_time_t mbedtls_ms_time (void)
{
	return static_cast<mbedtls_ms_time_t> (CTimer::GetClockTicks64 () / (CLOCKHZ / 1000));
}
