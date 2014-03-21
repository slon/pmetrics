env = Environment(tools=["default", "gmock"])

env.Append(CPPPATH=["."])
env.Append(CPPFLAGS=["-g", "-std=c++0x", "-Wall", "-Werror", "-O3", "-fPIC"])
env.Append(LINKFLAGS=["-g"])

pmetrics = env.Library("pmetrics", Glob("pm/*.cpp"))

env.GMock("run_ut", Glob("test/*.cpp"),
          LIBS=[pmetrics])


ut = env.Command('tbuild', '', './run_ut')
AlwaysBuild(ut)
