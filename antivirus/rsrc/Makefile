all:
	../bin/database_packer signatures.db ../bin/signatures_encrypted.db
	../bin/database_injector ../bin/signatures_encrypted.db ../bin/antivirus ../bin/antivirus_packed
	chmod u+x ../bin/antivirus_packed

clean:
	rm -f ../bin/signatures.db ../bin/signatures_encrypted.db
	rm -f ../bin/antivirus_packed
