#pragma once

#include "../common/common.h"
#include "../utils/csp.h"

#include <string.h>

#ifndef OPENSSL_NO_CTGOSTCP
	#include <openssl/opensslconf.h>
	#include <openssl/crypto.h>
	#include <openssl/ctgostcp.h>
	#include <openssl/ctcrypto.h>
#endif

#include "pkistore.h"

class ProviderCryptopro : public Provider{
public:
	ProviderCryptopro();
	~ProviderCryptopro(){};

public:
	Handle<Certificate> static getCert(Handle<std::string> hash, Handle<std::string> category);
	Handle<CRL> static getCRL(Handle<std::string> hash, Handle<std::string> category);
	Handle<Key> static getKey(Handle<Certificate> cert);

	static void addPkiObject(Handle<Certificate> cert, Handle<std::string> category, Handle<std::string> contName = new std::string(), int provType = NULL);
	static void deletePkiObject(Handle<Certificate> cert, Handle<std::string> category);
	static void deletePkiObject(Handle<CRL> crl, Handle<std::string> category);

	bool static hasPrivateKey(Handle<Certificate> cert);

private:
	void init();
	void enumCertificates(HCERTSTORE hCertStore, std::string *category);
	void enumCrls(HCERTSTORE hCertStore, std::string *category);
	Handle<PkiItem> objectToPKIItem(Handle<Certificate> cert);
	Handle<PkiItem> objectToPKIItem(Handle<CRL> crl);

	int static char2int(char input);
	void static hex2bin(const char* src, char* target);

#ifdef CSP_ENABLE
	bool static hasPrivateKey(PCCERT_CONTEXT pCertContext);
#endif //CSP_ENABLE
};
