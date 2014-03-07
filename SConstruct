env = Environment()

env.Append(CPPPATH=["."])
env.Append(CPPFLAGS=["-g", "-std=c++0x", "-Wall", "-Werror", "-O3", "-fPIC"])
env.Append(LINKFLAGS=["-g"])

pmetrics = env.Library("pmetrics", Glob("pm/*.cpp"))
