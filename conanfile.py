from conan import ConanFile
from conan.tools.files import copy

class ConanRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "PremakeDeps"

    def requirements(self):
        self.requires("assimp/5.4.3")
        self.requires("glfw/3.4")
        self.requires("glad/0.1.36")
        self.requires("glm/1.0.1")
        self.requires("imgui/1.91.5")
        self.requires("freetype/2.11.1")
        self.requires("stb/cci.20230920")
        self.requires("entt/3.14.0")
        self.requires("spdlog/1.11.0")

    def configure(self):
        self.options['glad'].gl_version = '4.6'
        self.options['glad'].extensions = [
            "GL_ARB_ES2_compatibility",
            "GL_ARB_direct_state_access",
            "GL_ARB_compute_shader",
            "GL_ARB_shader_image_load_store",
            "GL_ARB_shader_storage_buffer_object",
            "GL_ARB_copy_image",
            "GL_ARB_texture_storage",
            "GL_ARB_texture_multisample",
            "GL_ARB_invalidate_subdata",
            "GL_ARB_multi_bind",
            "GL_ARB_multi_draw_indirect"
            # Add additional extensions as needed
        ]