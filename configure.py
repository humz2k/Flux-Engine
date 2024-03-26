import os
import sys


print("Configuring ODE")
os.chdir("ext/ODE")
os.system("./configure")
os.chdir("../../")
print("Configured ODE")

if os.name != "nt":
    print("Configuring enet for OSX")
    os.chdir("ext/enet")
    os.system("./configure")
    os.chdir("../../")
    print("Configured enet for OSX")

#with open("FLUX_CONFIGURED","w") as f:
#    f.write("done")