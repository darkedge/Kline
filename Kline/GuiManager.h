#pragma once

namespace kl {
	class GuiManager {
	public:
		static void Init();
		static void Update();
		static void Render();
		static void Destroy();

		static void KeyCallBack(int key, int action, int mods);

		static float offset; // debug

	private:
		static void ImImpl_RenderDrawLists(ImDrawList** const cmd_lists, int cmd_lists_count);
		static const char* ImImpl_GetClipboardTextFn();
		static void ImImpl_SetClipboardTextFn(const char* text);
		static GLuint vertexBuffer;
		static GLuint handle;
		static GLuint fontTex;
		static GLuint vao;

		static struct Uniforms {
			GLint mvp_matrix;
		} uniforms;
	};
}
