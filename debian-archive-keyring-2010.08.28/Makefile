build: verify-indices keyrings/debian-archive-keyring.gpg keyrings/debian-archive-removed-keys.gpg verify-results

verify-indices: keyrings/team-members.gpg
	gpg --no-default-keyring --keyring keyrings/team-members.gpg \
		--verify active-keys/index.gpg active-keys/index
	gpg --no-default-keyring --keyring keyrings/team-members.gpg \
		--verify removed-keys/index.gpg removed-keys/index

verify-results: keyrings/team-members.gpg keyrings/debian-archive-keyring.gpg keyrings/debian-archive-removed-keys.gpg
	gpg --no-default-keyring --keyring keyrings/team-members.gpg --verify \
		 keyrings/debian-archive-keyring.gpg.asc \
		 keyrings/debian-archive-keyring.gpg
	gpg --no-default-keyring --keyring keyrings/team-members.gpg --verify \
		 keyrings/debian-archive-removed-keys.gpg.asc \
		 keyrings/debian-archive-removed-keys.gpg

keyrings/debian-archive-keyring.gpg: active-keys/index
	jetring-build -I $@ active-keys

keyrings/debian-archive-removed-keys.gpg: removed-keys/index
	jetring-build -I $@ removed-keys

keyrings/team-members.gpg: team-members/index
	jetring-build -I $@ team-members

clean:
	rm -f keyrings/debian-archive-keyring.gpg \
		keyrings/debian-archive-keyring.gpg.lastchangeset
	rm -f keyrings/debian-archive-removed-keys.gpg \
		keyrings/debian-archive-removed-keys.gpg.lastchangeset
	rm -f keyrings/team-members.gpg \
		keyrings/team-members.gpg.lastchangeset

.PHONY: verify-indices clean

