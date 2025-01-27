from conan import ConanFile
from conan.tools.files import copy

class ConanRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PremakeDeps"

    def requirements(self):
        self.requires("spdlog/1.11.0")
        self.requires("edyn/1.2.1")
        self.requires("assimp/5.4.3")
        self.requires("glfw/3.4")
        self.requires("glad/0.1.36")
        self.requires("openal/1.22.2")
        self.requires("glm/1.0.1")
        self.requires("imgui/1.91.5")
        self.requires("freetype/2.11.1")
        self.requires("stb/cci.20230920")
        self.requires("eastl/3.21.12")