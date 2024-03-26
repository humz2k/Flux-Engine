import os
import sys

if os.name != "nt":
    print("Configuring ODE (OSX ONLY)")
    os.chdir("ext/ODE")
    os.system("./configure")
    os.chdir("../../")
    print("Configured ODE")

    print("Configuring enet for OSX")
    os.chdir("ext/enet")
    os.system("./configure")
    os.chdir("../../")
    print("Configured enet for OSX")

#with open("FLUX_CONFIGURED","w") as f:
#    f.write("done")