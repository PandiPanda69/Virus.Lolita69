# -*- coding: utf-8 -*-
import base64
import zlib
import marshal


print """

LLLLLLLLLLL                              lllllll   iiii          tttt                                         66666666  999999999
L:::::::::L                              l:::::l  i::::i      ttt:::t                                        6::::::6 99:::::::::99
L:::::::::L                              l:::::l   iiii       t:::::t                                       6::::::699:::::::::::::99
LL:::::::LL                              l:::::l              t:::::t                                      6::::::69::::::99999::::::9
  L:::::L                  ooooooooooo    l::::l iiiiiiittttttt:::::ttttttt      aaaaaaaaaaaaa            6::::::6 9:::::9     9:::::9
  L:::::L                oo:::::::::::oo  l::::l i:::::it:::::::::::::::::t      a::::::::::::a          6::::::6  9:::::9     9:::::9
  L:::::L               o:::::::::::::::o l::::l  i::::it:::::::::::::::::t      aaaaaaaaa:::::a        6::::::6    9:::::99999::::::9
  L:::::L               o:::::ooooo:::::o l::::l  i::::itttttt:::::::tttttt               a::::a       6::::::::6666699::::::::::::::9
  L:::::L               o::::o     o::::o l::::l  i::::i      t:::::t              aaaaaaa:::::a      6::::::::::::::6699999::::::::9
  L:::::L               o::::o     o::::o l::::l  i::::i      t:::::t            aa::::::::::::a      6::::::66666:::::6    9::::::9
  L:::::L               o::::o     o::::o l::::l  i::::i      t:::::t           a::::aaaa::::::a      6:::::6     6:::::6  9::::::9
  L:::::L         LLLLLLo::::o     o::::o l::::l  i::::i      t:::::t    tttttta::::a    a:::::a      6:::::6     6:::::6 9::::::9
LL:::::::LLLLLLLLL:::::Lo:::::ooooo:::::ol::::::li::::::i     t::::::tttt:::::ta::::a    a:::::a      6::::::66666::::::69::::::9
L::::::::::::::::::::::Lo:::::::::::::::ol::::::li::::::i     tt::::::::::::::ta:::::aaaa::::::a       66:::::::::::::669::::::9
L::::::::::::::::::::::L oo:::::::::::oo l::::::li::::::i       tt:::::::::::tt a::::::::::aa:::a        66:::::::::66 9::::::9
LLLLLLLLLLLLLLLLLLLLLLLL   ooooooooooo   lllllllliiiiiiii         ttttttttttt    aaaaaaaaaa  aaaa          666666666  99999999
"""

exec marshal.loads(zlib.decompress(base64.b64decode("__FINAL_PLACEHOLDER__")))

print "Unfortunately your computer's specifications do not allow you access to our service.\nExiting..."
