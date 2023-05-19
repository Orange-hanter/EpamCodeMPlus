from conan import ConanFile


class CloneAppRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"

    def requirements(self):
        self.requires("catch2/3.3.2")
        self.requires("asio/1.28.0")
        self.requires("argparse/2.9")

    def build_requirements(self):
        self.tool_requires("cmake/3.22.6")