#include "../stdafx.h"

#include "pkistore.h"

#include "provider_system.h"

PkiStore::PkiStore(Handle<std::string> json){
	LOGGER_FN();
	
	try{
		if (json.isEmpty()){
			THROW_EXCEPTION(0, PkiStore, NULL, "json empty");
		}
		
		cash = new CashJson(json);
		providers = new ProviderCollection();
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Cannot be constructed PkiStore(Handle<std::string> json)");
	}	
}

Handle<PkiItemCollection> PkiStore::find(Handle<Filter> filter){
	LOGGER_FN();

	try{
		Handle<PkiItemCollection> storeItemCollection = new PkiItemCollection();

		storeItemCollection = getItems();

		if (storeItemCollection.isEmpty()){
			THROW_EXCEPTION(0, PkiStore, NULL, "Store no have pki elements");
		}

		Handle<PkiItemCollection> filteredItems = new PkiItemCollection();

		for (int i = 0, c = storeItemCollection->length(); i < c; i++){
			bool result = 1;

			if (filter->types.size() > 0){
				result = 0;
				for (int j = 0; j < filter->types.size(); j++){
					if (strcmp(storeItemCollection->items(i)->type->c_str(), filter->types[j]->c_str()) == 0){
						result = 1;
						break;
					}
				}
				if (!result){
					continue;
				}
			}

			if (filter->providers.size() > 0){
				result = 0;
				for (int j = 0; j < filter->providers.size(); j++){
					if (strcmp(storeItemCollection->items(i)->provider->c_str(), filter->providers[j]->c_str()) == 0){
						result = 1;
						break;
					}
				}
				if (!result){
					continue;
				}
			}
			
			if (filter->categorys.size() > 0){
				result = 0;
				for (int j = 0; j < filter->categorys.size(); j++){
					if (strcmp(storeItemCollection->items(i)->category->c_str(), filter->categorys[j]->c_str()) == 0){
						result = 1;
						break;
					}
				}
				if (!result){
					continue;
				}
			}

			if (!(filter->hash.isEmpty())){
				if (strcmp(storeItemCollection->items(i)->hash->c_str(), filter->hash->c_str()) == 0){
					result = 1;
				}
				else{
					result = 0;
					continue;
				}
			}

			if (!(filter->subjectName.isEmpty())){
				if ((strcmp(storeItemCollection->items(i)->certSubjectName->c_str(), filter->subjectName->c_str()) == 0) ||
					(strcmp(storeItemCollection->items(i)->csrSubjectName->c_str(), filter->subjectName->c_str()) == 0)){
					result = 1;
				}
				else{
					result = 0;
					continue;
				}
			}

			if (!(filter->subjectFriendlyName.isEmpty())){
				if ((strcmp(storeItemCollection->items(i)->certSubjectFriendlyName->c_str(), filter->subjectFriendlyName->c_str()) == 0) ||
					(strcmp(storeItemCollection->items(i)->csrSubjectFriendlyName->c_str(), filter->subjectFriendlyName->c_str()) == 0)){
					result = 1;
				}
				else{
					result = 0;
					continue;
				}
			}

			if (!(filter->issuerName.isEmpty())){
				if ((strcmp(storeItemCollection->items(i)->certIssuerName->c_str(), filter->issuerName->c_str()) == 0) ||
					(strcmp(storeItemCollection->items(i)->crlIssuerName->c_str(), filter->issuerName->c_str()) == 0)){
					result = 1;
				}
				else{
					result = 0;
					continue;
				}
			}

			if (!(filter->issuerFriendlyName.isEmpty())){
				if ((strcmp(storeItemCollection->items(i)->certIssuerFriendlyName->c_str(), filter->issuerFriendlyName->c_str()) == 0) ||
					(strcmp(storeItemCollection->items(i)->crlIssuerFriendlyName->c_str(), filter->issuerFriendlyName->c_str()) == 0)){
					result = 1;
				}
				else{
					result = 0;
					continue;
				}
			}

			if (!(filter->serial.isEmpty())){
				if (strcmp(storeItemCollection->items(i)->certSerial->c_str(), filter->serial->c_str()) == 0){
					result = 1;
				}
				else{
					result = 0;
					continue;
				}
			}

			if (result){
				filteredItems->push(storeItemCollection->items(i));
			}
		}

		return filteredItems;
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error search object");
	}
}

Handle<PkiItem> PkiStore::findKey(Handle<Filter> filter){
	LOGGER_FN();

	try{
		if (filter->hash.isEmpty()){
			THROW_EXCEPTION(0, PkiStore, NULL, "Need hash certificate\request for search key");
		}

		Handle<PkiItemCollection> storeItemCollection = new PkiItemCollection();

		storeItemCollection = getItems();

		if (storeItemCollection.isEmpty()){
			THROW_EXCEPTION(0, PkiStore, NULL, "Store no have pki elements");
		}

		Handle<PkiItem> key = new PkiItem();

		for (int i = 0, c = storeItemCollection->length(); i < c; i++){
			bool result = 1;

			if (strcmp(storeItemCollection->items(i)->hash->c_str(), filter->hash->c_str()) == 0){
				result = 1;				
			}
			else{
				result = 0;
				continue;
			}

			if (result){
				Handle<std::string> keyHash;

				if (!(storeItemCollection->items(i)->certKey.isEmpty())){
					keyHash = storeItemCollection->items(i)->certKey;
				}
				else if (!(storeItemCollection->items(i)->csrKey.isEmpty())){
					keyHash = storeItemCollection->items(i)->csrKey;
				}
				else{
					THROW_EXCEPTION(0, PkiStore, NULL, "Object no have key");
				}

				for (int j = 0; j < storeItemCollection->length(); j++){
					if ((strcmp(storeItemCollection->items(j)->type->c_str(), "KEY") == 0) &&
						((strcmp(storeItemCollection->items(j)->hash->c_str(), keyHash->c_str()) == 0))){
						key = storeItemCollection->items(j);
						break;
					}
				}

				break;
			}
		}

		return key;
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error search key");
	}
}


Handle<Certificate> PkiStore::getItemCert(Handle<PkiItem> item){
	LOGGER_FN();

	try{
		Handle<Certificate> cert = new Certificate();

		if (strcmp(item->provider->c_str(), "SYSTEM") == 0){
			cert = Provider_System::getCertFromURI(item->uri, item->format);
		}
		else{
			THROW_EXCEPTION(0, PkiStore, NULL, "Provider type unsoported")
		}

		return cert;
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error get cert from store");
	}	
}

Handle<CRL> PkiStore::getItemCrl(Handle<PkiItem> item){
	LOGGER_FN();

	try{
		Handle<CRL> crl = new CRL();

		if (strcmp(item->provider->c_str(), "SYSTEM") == 0){
			crl = Provider_System::getCRLFromURI(item->uri, item->format);
		}
		else{
			THROW_EXCEPTION(0, PkiStore, NULL, "Provider type unsoported")
		}

		return crl;
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error get crl from store");
	}
}

Handle<CertificationRequest> PkiStore::getItemReq(Handle<PkiItem> item){
	LOGGER_FN();

	try{
		Handle<CertificationRequest> csr = new CertificationRequest();

		if (strcmp(item->provider->c_str(), "SYSTEM") == 0){
			csr = Provider_System::getCSRFromURI(item->uri, item->format);
		}
		else{
			THROW_EXCEPTION(0, PkiStore, NULL, "Provider type unsoported")
		}

		return csr;
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error get crl from store");
	}
}

Handle<Key> PkiStore::getItemKey(Handle<PkiItem> item){
	LOGGER_FN();

	try{
		Handle<Key> key = new Key();

		if (strcmp(item->provider->c_str(), "SYSTEM") == 0){
			key = Provider_System::getKeyFromURI(item->uri, item->format, item->keyEncrypted);
		}
		else{
			THROW_EXCEPTION(0, PkiStore, NULL, "Provider type unsoported")
		}

		return key;
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error get key from store");
	}
}


Handle<PkiItemCollection> PkiStore::getItems(){
	LOGGER_FN();

	try{
		Handle<PkiItemCollection> result = new PkiItemCollection();

		if (providers->length() == 0){
			THROW_EXCEPTION(0, PkiStore, NULL, "Collection providers empty");
		}

		for (int i = 0; i < providers->length(); i++){
			Handle<PkiItemCollection> tempColl;

			tempColl = providers->items(i)->getProviderItemCollection();

			for (int j = 0; j < tempColl->length(); j++){
				result->push(tempColl->items(j));
			}
		}

		return result;
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error get items from store");
	}	
}

void PkiStore::addProvider(Handle<Provider> provider){
	LOGGER_FN();
	
	providers->push(provider);
}

void PkiStore::addPkiObject(Handle<Provider> provider, Handle<std::string> category, Handle<Certificate> cert, unsigned int flags){
	LOGGER_FN();

	try{
		if (strcmp(provider->type->c_str(), "SYSTEM") == 0){
			std::string uri = (std::string)provider->path->c_str() + CROSSPLATFORM_SLASH + (std::string)category->c_str() + CROSSPLATFORM_SLASH;

			char * hexHash;
			Handle<std::string> hhash = cert->getThumbprint();
			Provider_System::bin_to_strhex((unsigned char *)hhash->c_str(), hhash->length(), &hexHash);

			uri = uri + std::string(hexHash) + "_";

			EVP_PKEY *pkey;
			LOGGER_OPENSSL(BIO_new);
			BIO * bioBN = BIO_new(BIO_s_mem());
			LOGGER_OPENSSL(X509_get_pubkey);
			pkey = X509_get_pubkey(cert->internal());
			if (pkey == NULL) {
				THROW_OPENSSL_EXCEPTION(0, PkiStore, NULL, "Modulus=unavailable", NULL);
			}

			#ifndef OPENSSL_NO_RSA
			if (pkey->type == EVP_PKEY_RSA)
				BN_print(bioBN, pkey->pkey.rsa->n);
			else
			#endif
			#ifndef OPENSSL_NO_DSA
				if (pkey->type == EVP_PKEY_DSA)
					BN_print(bioBN, pkey->pkey.dsa->pub_key);
				else
			#endif

			LOGGER_OPENSSL(EVP_PKEY_free);
			EVP_PKEY_free(pkey);
			
			int contlen;
			char * cont;
			LOGGER_OPENSSL(BIO_get_mem_data);
			contlen = BIO_get_mem_data(bioBN, &cont);

			unsigned char tmphash[SHA_DIGEST_LENGTH];
			LOGGER_OPENSSL(SHA1);
			SHA1((const unsigned char *)cont, contlen, tmphash);
			Provider_System::bin_to_strhex(tmphash, SHA_DIGEST_LENGTH, &hexHash);

			uri = uri + std::string(reinterpret_cast<char*>(hexHash)) + ".crt";

			Handle<std::string> huri = new std::string(uri);
 			Provider_System::addPkiObject(huri, cert, flags);
		}
		else{
			THROW_EXCEPTION(0, PkiStore, NULL, "Provider type unsoported")
		}
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error add certificate to store");
	}
}

void PkiStore::addPkiObject(Handle<Provider> provider, Handle<std::string> category, Handle<CRL> crl, unsigned int flags){
	LOGGER_FN();

	try{
		if (strcmp(provider->type->c_str(), "SYSTEM") == 0){
			std::string uri = (std::string)provider->path->c_str() + CROSSPLATFORM_SLASH + (std::string)category->c_str() + CROSSPLATFORM_SLASH;

			char * hexHash;
			Handle<std::string> hhash = crl->getThumbprint();
			Provider_System::bin_to_strhex((unsigned char *)hhash->c_str(), hhash->length(), &hexHash);

			uri = uri + std::string(hexHash) + ".crl";

			Handle<std::string> huri = new std::string(uri);
			Provider_System::addPkiObject(huri, crl, flags);
		}
		else{
			THROW_EXCEPTION(0, PkiStore, NULL, "Provider type unsoported")
		}
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error add crl to store");
	}
}

void PkiStore::addPkiObject(Handle<Provider> provider, Handle<std::string> category, Handle<CertificationRequest> csr){
	LOGGER_FN();

	try{
		if (strcmp(provider->type->c_str(), "SYSTEM") == 0){
			std::string uri = (std::string)provider->path->c_str() + CROSSPLATFORM_SLASH + (std::string)category->c_str() + CROSSPLATFORM_SLASH;

			unsigned char hash[EVP_MAX_MD_SIZE] = { 0 };
			unsigned int hashlen = 0;
			LOGGER_OPENSSL(X509_digest);
			if (!X509_REQ_digest(csr->internal(), EVP_sha1(), hash, &hashlen)) {
				THROW_OPENSSL_EXCEPTION(0, Provider_System, NULL, "X509_REQ_digest");
			}
			Handle<std::string> hhash = new std::string((char *)hash, hashlen);

			char * hexHash;
			Provider_System::bin_to_strhex((unsigned char *)hhash->c_str(), hhash->length(), &hexHash);

			uri = uri + std::string(reinterpret_cast<char*>(hexHash)) + "_";

			EVP_PKEY *pkey;
			LOGGER_OPENSSL(BIO_new);
			BIO * bioBN = BIO_new(BIO_s_mem());
			LOGGER_OPENSSL(X509_REQ_get_pubkey);
			pkey = X509_REQ_get_pubkey(csr->internal());
			if (pkey == NULL) {
				THROW_OPENSSL_EXCEPTION(0, PkiStore, NULL, "Modulus=unavailable", NULL);
			}

			#ifndef OPENSSL_NO_RSA
			if (pkey->type == EVP_PKEY_RSA)
				BN_print(bioBN, pkey->pkey.rsa->n);
			else
				#endif
			#ifndef OPENSSL_NO_DSA
				if (pkey->type == EVP_PKEY_DSA)
					BN_print(bioBN, pkey->pkey.dsa->pub_key);
				else
					#endif

					LOGGER_OPENSSL(EVP_PKEY_free);
			EVP_PKEY_free(pkey);

			int contlen;
			char * cont;
			LOGGER_OPENSSL(BIO_get_mem_data);
			contlen = BIO_get_mem_data(bioBN, &cont);

			unsigned char tmphash[SHA_DIGEST_LENGTH];
			LOGGER_OPENSSL(SHA1);
			SHA1((const unsigned char *)cont, contlen, tmphash);
			Provider_System::bin_to_strhex(tmphash, SHA_DIGEST_LENGTH, &hexHash);

			uri = uri + std::string(reinterpret_cast<char*>(hexHash)) + ".csr";

			Handle<std::string> huri = new std::string(uri);
			Provider_System::addPkiObject(huri, csr);
		}
		else{
			THROW_EXCEPTION(0, PkiStore, NULL, "Provider type unsoported")
		}
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error add csr to store");
	}
}

void PkiStore::addPkiObject(Handle<Provider> provider, Handle<Key> key, Handle<std::string> password){
	LOGGER_FN();

	try{
		if (strcmp(provider->type->c_str(), "SYSTEM") == 0){
			std::string uri = (std::string)provider->path->c_str() + CROSSPLATFORM_SLASH + "MY" + CROSSPLATFORM_SLASH;

			RSA *rsa = NULL;
			LOGGER_OPENSSL(EVP_PKEY_get1_RSA);
			rsa = EVP_PKEY_get1_RSA(key->internal());

			LOGGER_OPENSSL(BIO_new);
			BIO * bioBN = BIO_new(BIO_s_mem());
			BN_print(bioBN, rsa->n);

			int contlen;
			char * cont;
			LOGGER_OPENSSL(BIO_get_mem_data);
			contlen = BIO_get_mem_data(bioBN, &cont);

			char * hexHash;
			unsigned char tmphash[SHA_DIGEST_LENGTH];
			LOGGER_OPENSSL(SHA1);
			SHA1((const unsigned char *)cont, contlen, tmphash);
			Provider_System::bin_to_strhex(tmphash, SHA_DIGEST_LENGTH, &hexHash);
									
			uri = uri + std::string(hexHash) + ".key";

			Handle<std::string> huri = new std::string(uri);
			Provider_System::addPkiObject(huri, key, password);
		}
		else{
			THROW_EXCEPTION(0, PkiStore, NULL, "Provider type unsoported")
		}
	}
	catch (Handle<Exception> e){
		THROW_EXCEPTION(0, PkiStore, e, "Error add key to store");
	}
}