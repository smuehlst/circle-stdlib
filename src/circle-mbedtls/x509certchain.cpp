//
// x509certchain.cpp
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
#include <circle-mbedtls/x509certchain.h>
#include <mbedtls/x509.h>
#include <mbedtls/pk.h>
#include <circle/logger.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <assert.h>

#define CERTIFICATE_MAX_SIZE	5000

using namespace CircleMbedTLS;

int CX509CertificateChain::AddCertificateFile (const char *pFilename)
{
	assert (pFilename != 0);
	FILE *pFile = fopen (pFilename, "rb");
	if (pFile == 0)
	{
		return MBEDTLS_ERR_PK_FILE_IO_ERROR;
	}

	u8 Buffer[CERTIFICATE_MAX_SIZE+1];
	size_t nLength = fread (Buffer, 1, CERTIFICATE_MAX_SIZE, pFile);

	fclose (pFile);

	if (nLength == 0)
	{
		return MBEDTLS_ERR_PK_FILE_IO_ERROR;
	}
	if (nLength >= CERTIFICATE_MAX_SIZE)
	{
		return MBEDTLS_ERR_X509_BUFFER_TOO_SMALL;
	}

	Buffer[nLength] = '\0';

	if (strstr ((const char *) Buffer, "-----BEGIN ") != 0)
	{
		nLength++;
	}

	return AddCertificate (Buffer, nLength);
}

int CX509CertificateChain::AddCertificatePath (const char *pPath)
{
	assert (pPath != 0);
	DIR *pDirectory = opendir (pPath);
	if (pDirectory == 0)
	{
		return MBEDTLS_ERR_PK_FILE_IO_ERROR;
	}

	int nFiles = 0;

	struct dirent *pEntry;
	while ((pEntry = readdir (pDirectory)) != 0)
	{
		size_t nLength = strlen (pEntry->d_name);
		if (nLength < 5)
		{
			continue;
		}
		nLength -= 4;

		if (   strcmp (&pEntry->d_name[nLength], ".pem") != 0
		    && strcmp (&pEntry->d_name[nLength], ".crt") != 0
		    && strcmp (&pEntry->d_name[nLength], ".der") != 0)
		{
			continue;
		}

		int nResult = AddCertificateFile (pEntry->d_name);
		if (nResult < 0)
		{
			CLogger::Get ()->Write ("x509cert", LogWarning,
						"Cannot parse file: %s (%d)",
						pEntry->d_name, nResult);

			closedir (pDirectory);

			return nResult;
		}

		nFiles++;
	}

	closedir (pDirectory);

	return nFiles;
}
