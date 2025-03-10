from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps


class scRecipe(ConanFile):
    name = "sc"
    version = "0.0.1"
    package_type = "library"

    # Optional metadata
    license = "<Put the package license here>"
    author = "<Put your name here> <And your email here>"
    url = "<Package recipe repository url here, for issues about the package>"
    description = "<Description of sc package here>"
    topics = ("<Put some tag here>", "<here>", "<and here>")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"
    options = {"shared": [True, False], "fPIC": [True, False], "STATIC_ANALYSIS": [True, False], "STRICT_WARNINGS": [True, False]}
    default_options = {"shared": False, "fPIC": True, "STATIC_ANALYSIS": False, "STRICT_WARNINGS": True}

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = ".clang-tidy", ".clang-format", "CppCheckSuppressions.txt", "CMakeLists.txt", "cmake-files/*", "example/*", "docs/*", "include/*", "src/*", "tests/*"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.variables["STATIC_ANALYSIS"] = self.options.STATIC_ANALYSIS
        tc.variables["STRICT_WARNINGS"] = self.options.STRICT_WARNINGS
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["sc"]

    def requirements(self):
        self.requires("spdlog/1.15.0")
        

    def build_requirements(self):
        self.requires("gtest/1.15.0")
    