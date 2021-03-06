'''Logging module
'''
# ------------------------------------------------------------------------------
# IMPORTS
# ------------------------------------------------------------------------------
from logging import getLogger, basicConfig, INFO, DEBUG

# ------------------------------------------------------------------------------
# GLOBALS
# ------------------------------------------------------------------------------
basicConfig(format="(%(name)s)[%(levelname)7s]: %(message)s", level=DEBUG)
app_log = getLogger('hbsrecover')
app_log.setLevel(INFO)
# ------------------------------------------------------------------------------
# FUNCTIONS
# ------------------------------------------------------------------------------
def set_debug(debug=True):
    app_log.setLevel(DEBUG if debug else INFO)
