# varexp/cfg.mk

gnu_rel_host    := dl.sv.nongnu.org
upload_dest_dir_:= /releases/varexp/
old_NEWS_hash   := d41d8cd98f00b204e9800998ecf8427e
gpg_key_ID      := 99089D72
url_dir_list    := http://download.savannah.nongnu.org/releases/varexp
today           := $(date "+%Y-%m-%d")
TAR_OPTIONS     += --mtime=$(today)
