"""Simple configure checks for boost"""

from SCons.Script import Exit
import checks

def _check(context, version):
    # Boost versions are in format major.minor.subminor
    v_arr = version.split(".")
    version_n = 0
    if len(v_arr) > 0:
        version_n += int(v_arr[0])*100000
    if len(v_arr) > 1:
        version_n += int(v_arr[1])*100
    if len(v_arr) > 2:
        version_n += int(v_arr[2])

    context.Message('Checking for Boost version >= %s... ' % (version))
    ret = context.TryRun("""#include <boost/version.hpp>
#include <iostream>

        int main()
        {
            std::cout << BOOST_LIB_VERSION <<std::endl;
            std::cout << BOOST_VERSION << std::endl;
            return BOOST_VERSION >= %d ? 0 : 1;
        }
        """ % version_n, '.cpp')
    if ret[0]:
        context.Result(ret[1].replace("_", ".").split('\n')[0])
        context.env['BOOST_VERSION']= ret[1].split('\n')[1]
    return ret[0]

def configure_check(env, version):
    if env.get('boostversion', None):
        env['BOOST_VERSION']=env['boostversion']
    else:
        custom_tests = {'CheckBoost':_check}
        conf = env.Configure(custom_tests=custom_tests)
        conf.CheckBoost(version)
        conf.Finish()
