from conan import ConanFile
from conan.tools.cmake import (
    CMakeDeps,
    CMakeToolchain,
    cmake_layout,
)


class PostboyConan(ConanFile):
    settings = [
        'os',
        'arch',
        'compiler',
        'build_type',
    ]
    default_options = {
        'libwebsockets/*:shared': False,
        'libwebsockets/*:with_libevent': 'libev',
        'libwebsockets/*:role_ws': True,
        'libwebsockets/*:enable_extensions': True,
    }

    def requirements(self):
        self.requires('benchmark/1.9.1')
        self.requires('boost/1.81.0')
        self.requires('cpp-httplib/0.19.0')
        self.requires('libcurl/8.6.0')
        self.requires('libev/4.33')
        self.requires('libwebsockets/4.3.3')
        self.requires('magic_enum/0.9.7')
        self.requires('openssl/1.1.1w')
        self.requires('quill/7.5.0')

    def build_requirements(self):
        self.build_requires('cmake/[>=3.24]')
        self.build_requires('ninja/[>=1.11.1]')

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()

        tc = CMakeToolchain(self)
        tc.user_presets_path = 'ConanPresets.json'
        tc.generate()

    def layout(self):
        cmake_layout(self)
