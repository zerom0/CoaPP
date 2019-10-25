from conans import ConanFile, CMake, tools


class CoappConan(ConanFile):
    name = "CoaPP"
    version = "1.0"
    license = "MPL-2.0"
    author = "zerom0 github@martinmosler.de"
    url = "https://github.com/zerom0/CoaPP"
    description = "C++11 implementation of the CoAP"
    topics = ("coap")
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    exports_sources = "*"
    requires = "gtest/1.8.1@bincrafters/stable"

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("coap_client", src="bin", dst="bin")
        self.copy("coap_server", src="bin", dst="bin")
        self.copy("*.so", src="lib", dst="lib")
        self.copy("*.h", src="coap/include", dst="include")

    def package_info(self):
        self.cpp_info.libs = ["coap"]

