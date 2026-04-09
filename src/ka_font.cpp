#include <glad/glad.h>
#include <ka_font>
#include <print>
#include <ft2build.h>
#include <freetype/freetype.h>

namespace Kawai
{
    Font::~Font()
    {
        cleanup();
    }

    bool Font::load(const std::string &fontPath, uint32_t fontSize)
    {
        if (m_Loaded)
            cleanup();

        FT_Library ft;
        if (FT_Init_FreeType(&ft))
        {
            std::println("freetype 初始化失败!!!");
            return false;
        }

        FT_Face face;
        if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
        {
            std::println("加载字体失败!!!");
            FT_Done_FreeType(ft);
            return false;
        }

        printf("FreeType version: %d.%d.%d\n",
       FREETYPE_MAJOR,
       FREETYPE_MINOR,
       FREETYPE_PATCH);

        FT_Set_Pixel_Sizes(face, 0, fontSize);

        // ⭐ 必须设置像素对齐
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        for (unsigned char c = 0; c < 128; c++)
        {
            if (FT_Load_Char(face, c, FT_LOAD_DEFAULT))
                continue;

            if (FT_Render_Glyph(face->glyph, FT_RENDER_MODE_SDF))
                continue;

            FT_Bitmap &bmp = face->glyph->bitmap;

            GLuint tex;
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);

            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                         bmp.width,
                         bmp.rows,
                         0, GL_RED, GL_UNSIGNED_BYTE,
                         bmp.buffer);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            Character ch = {
                tex,
                glm::ivec2(bmp.width, bmp.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                (unsigned int)face->glyph->advance.x};

            m_Characters[c] = ch;
        }

        FT_Done_Face(face);
        FT_Done_FreeType(ft);

        m_FontSize = fontSize;
        m_Loaded = true;
        std::println("字体加载成功!!!");
        return true;
    }

    void Font::cleanup()
    {
        if (!m_Loaded)
            return;
        for (auto &ch : m_Characters)
            glDeleteTextures(1, &ch.second.tex_ID);
        m_Characters.clear();
        m_Loaded = false;
    }

    Character Font::GetCharacter(char c) const
    {
        auto it = m_Characters.find(c);
        if (it != m_Characters.end())
            return it->second;
        return {};
    }
}
