# This file has been dedicated to the public domain, to the extent
# possible under applicable law, via CC0. See
# http://creativecommons.org/publicdomain/zero/1.0/ for more
# information. This file is offered as-is, without any warranty.

SUBDIRS = src misc

dist_doc_DATA = README.txt COPYING LICENSES

# WHEN ADDING TRANSLATIONS: Please append all new gettext file names
# here (both the .po file and the .mo file).  This insures the
# translation is both distributed and installed properly.
nobase_dist_pkgdata_DATA = data/* gfx/* music/* sound/* \
	locale/build.py \
	locale/genpot.sh \
	locale/pr-starfighter.pot \
	locale/en.po \
	locale/en/LC_MESSAGES/pr-starfighter.mo \
	locale/ja.po \
	locale/ja/LC_MESSAGES/pr-starfighter.mo

EXTRA_DIST = autogen.sh win32build.sh misc
