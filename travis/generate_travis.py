# Generate .travis.yml automatically

# Configuration for Linux
configs_linux = [
    # OS, compiler, build type
    ("debian-9", "gcc", "DefaultDebug"),
    ("debian-9", "gcc", "DefaultRelease"),
    ("ubuntu-17.10", "gcc", "DefaultDebugTravis"),
    ("ubuntu-17.10", "gcc", "DefaultReleaseTravis"),
]

# Configurations for Mac
configs_mac = [
    # OS, compiler, build type
#    ("osx", "clang", "DefaultDebug"),
#    ("osx", "clang", "DefaultRelease"),
]

# Stages in travis
stages = [
    ("Build (1st run)", "Build1"),
    ("Build (2nd run)", "Build2"),
    ("Build (3rd run)", "Build3"),
    ("Build (4th run)", "BuildLast"),
    ("Test all", "TestAll"),
]


if __name__ == "__main__":
    s = ""
    # Initial config
    s += "#\n"
    s += "# General config\n"
    s += "#\n"
    s += "branches:\n"
    s += "  only:\n"
    s += "  - master\n"
    s += "  - stable\n"
    s += "sudo: required\n"
    s += "dist: trusty\n"
    s += "language: cpp\n"
    s += "\n"
    s += "# Enable caching\n"
    s += "cache:\n"
    s += "  timeout: 1000\n"
    s += "  directories:\n"
    s += "  - build\n"
    s += "  - travis/mtime_cache\n"
    s += "\n"
    s += "# Enable docker support\n"
    s += "services:\n"
    s += "- docker\n"
    s += "\n"

    s += "notifications:\n"
    s += "  email:\n"
    s += "    on_failure: always\n"
    s += "    on_success: change\n"
    s += "    recipients:\n"
    s += '    secure: "Q9CW/PtyWkZwExDrfFFb9n1STGYsRfI6awv1bZHcGwfrDvhpXoMCuF8CwviIfilm7fFJJEoKizTtdRpY0HrOnY/8KY111xrtcFYosxdndv7xbESodIxQOUoIEFCO0mCNHwWYisoi3dAA7H3Yn661EsxluwHjzX5vY0xSbw0n229hlsFz092HwGLSU33zHl7eXpQk+BOFmBTRGlOq9obtDZ17zbHz1oXFZntHK/JDUIYP0b0uq8NvE2jM6CIVdcuSwmIkOhZJoO2L3Py3rBbPci+L2PSK4Smv2UjCPF8KusnOaFIyDB3LcNM9Jqq5ssJMrK/KaO6BiuYrOZXYWZ7KEg3Y/naC8HjOH1dzty+P7oW46sb9F03pTsufqD4R7wcK+9wROTztO6aJPDG/IPH7EWgrBTxqlOkVRwi2eYuQouZpZUW6EMClKbMHMIxCH2S8aOk/r1w2cNwmPEcunnP0nl413x/ByHr4fTPFykPj8pQxIsllYjpdWBRQfDOauKWGzk6LcrFW0qpWP+/aJ2vYu/IoZQMG5lMHbp6Y1Lg09pYm7Q983v3b7D+JvXhOXMyGq91HyPahA2wwKoG1GA4uoZ2I95/IFYNiKkelDd3WTBoFLNF9YFoEJNdCywm1fO2WY4WkyEFBuQcgDA+YpFMJJMxjTbivYk9jvHk2gji//2w="\n'
    s += "\n"
    s += "#\n"
    s += "# Configurations\n"
    s += "#\n"
    s += "jobs:\n"
    s += "  include:\n"

    # Start with prebuilding carl for docker
    s += "\n"
    s += "    ###\n"
    s += "    # Stage: Build Carl\n"
    s += "    ###\n"
    s += "\n"
    for config in configs_linux:
        linux = config[0]
        compiler = config[1]
        build_type = config[2]
        if "Travis" in build_type:
            s += "    # {} - {}\n".format(linux, build_type)
            buildConfig = ""
            buildConfig += "    - stage: Build Carl\n"
            buildConfig += "      os: linux\n"
            buildConfig += "      compiler: {}\n".format(compiler)
            buildConfig += "      env: CONFIG={} LINUX={} COMPILER={}\n".format(build_type, linux, compiler)
            buildConfig += "      install:\n"
            buildConfig += "        - travis/install_linux.sh\n"
            buildConfig += "      script:\n"
            buildConfig += "        - travis/build_carl.sh\n"
            # Upload to DockerHub
            buildConfig += "      after_success:\n"
            buildConfig += '        - docker login -u "$DOCKER_USERNAME" -p "$DOCKER_PASSWORD";\n'
            if "Debug" in build_type:
                buildConfig += "        - docker commit carl mvolk/carl:travis-debug;\n"
                buildConfig += "        - docker push mvolk/carl:travis-debug;\n"
            elif "Release" in build_type:
                buildConfig += "        - docker commit carl mvolk/carl:travis;\n"
                buildConfig += "        - docker push mvolk/carl:travis;\n"
            else:
                assert False
            s += buildConfig

    # Generate all configurations
    for stage in stages:
        s += "\n"
        s += "    ###\n"
        s += "    # Stage: {}\n".format(stage[0])
        s += "    ###\n"
        s += "\n"
        # Mac OS X
        for config in configs_mac:
            osx = config[0]
            compiler = config[1]
            build_type = config[2]
            s += "    # {} - {}\n".format(osx, build_type)
            buildConfig = ""
            buildConfig += "    - stage: {}\n".format(stage[0])
            buildConfig += "      os: osx\n"
            buildConfig += "      osx_image: xcode9.1\n"
            buildConfig += "      compiler: {}\n".format(compiler)
            buildConfig += "      env: CONFIG={} COMPILER={} STL=libc++\n".format(build_type, compiler)
            buildConfig += "      install:\n"
            if stage[1] == "Build1":
                buildConfig += "        - rm -rf build\n"
            buildConfig += "        - travis/install_osx.sh\n"
            buildConfig += "      script:\n"
            buildConfig += "        - travis/build.sh {}\n".format(stage[1])
            buildConfig += "      after_failure:\n"
            buildConfig += "        - find build -iname '*err*.log' -type f -print -exec cat {} \;\n"
            s += buildConfig

        # Linux via Docker
        for config in configs_linux:
            linux = config[0]
            compiler = config[1]
            build_type = config[2]
            s += "    # {} - {}\n".format(linux, build_type)
            buildConfig = ""
            buildConfig += "    - stage: {}\n".format(stage[0])
            buildConfig += "      os: linux\n"
            buildConfig += "      compiler: {}\n".format(compiler)
            buildConfig += "      env: CONFIG={} LINUX={} COMPILER={}\n".format(build_type, linux, compiler)
            buildConfig += "      install:\n"
            if stage[1] == "Build1":
                buildConfig += "        - rm -rf build\n"
            buildConfig += "        - travis/install_linux.sh\n"
            buildConfig += "      script:\n"
            buildConfig += "        - travis/build.sh {}\n".format(stage[1])
            buildConfig += "      before_cache:\n"
            buildConfig += "        - docker cp storm:/opt/storm/. .\n"
            buildConfig += "      after_failure:\n"
            buildConfig += "        - find build -iname '*err*.log' -type f -print -exec cat {} \;\n"
            # Upload to DockerHub
            if stage[1] == "TestAll" and "Travis" in build_type:
                buildConfig += "      after_success:\n"
                buildConfig += '        - docker login -u "$DOCKER_USERNAME" -p "$DOCKER_PASSWORD";\n'
                if "Debug" in build_type:
                    buildConfig += "        - docker commit storm mvolk/storm:travis-debug;\n"
                    buildConfig += "        - docker push mvolk/storm:travis-debug;\n"
                elif "Release" in build_type:
                    buildConfig += "        - docker commit storm mvolk/storm:travis;\n"
                    buildConfig += "        - docker push mvolk/storm:travis;\n"
                else:
                    assert False
            s += buildConfig

    print(s)
