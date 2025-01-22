from conan import ConanFile

class ConanTutorialRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PremakeDeps"

    def requirements(self):
        self.requires("spdlog/1.11.0")
        self.requires("edyn/1.2.1")
        self.requires("assimp/5.4.3")
        self.requires("glfw/3.4")
        self.requires("glad/0.1.36")
        self.requires("openal/1.22.2")
        self.requires("glm/cci.20230113")
        self.requires("imgui/cci.20230105+1.89.2.docking")
        self.requires("freetype/2.13.3")
        self.requires("stb/cci.20230920")