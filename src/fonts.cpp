#include "fonts.h"
#include <ft2build.h>
#include FT_FREETYPE_H 
#include <freetype/ftglyph.h>

extern FT_Library library;

int next_p2(int v) {
	v--;
	v |= v >> 1;
	v |= v >> 2;
	v |= v >> 4;
	v |= v >> 8;
	v |= v >> 16;
	v++;
	return v;
}

void font::load () {
	FT_Face     ft_face;
	auto error = FT_New_Face( 	library,
					"./Fipps-Regular.otf",
					0, &ft_face);
	if ( error == FT_Err_Unknown_File_Format )
	{
		printf("could not load font\n");
	}
	FT_Set_Char_Size( ft_face, 18<< 6, 18 << 6, 96, 96);
	//Initialize the char_info array
	info.ch.resize(ft_face->num_glyphs);
	info.max_height = 0;

	// Variables to hold the size the texture must have to hold all the bitmaps
	int max_width = 0;
	int max_rows = 0;

	// Create all the characters
	int y = 0;
	for(unsigned int ch = 0; y < ft_face->num_glyphs; ch++)
	{
	    //Load the Glyph for our character.
	    if(FT_Load_Glyph( ft_face, FT_Get_Char_Index( ft_face, ch ), FT_LOAD_NO_HINTING ))
	    {
	        throw std::runtime_error("FT_Load_Glyph failed");
	    };

	    //Move the face's glyph into a Glyph object.
	    FT_Glyph glyph;
	    if(FT_Get_Glyph( ft_face->glyph, &glyph ))
	    {
	        throw std::runtime_error("FT_Get_Glyph failed");
	    }
			y++;
	    //Convert the glyph to a bitmap.
	    FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, 0, 1 );
	    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;

	    //This reference will make accessing the bitmap easier
	    FT_Bitmap& bitmap = bitmap_glyph->bitmap;

	    //Get a pointer to the char info for easy access
	    font::char_info_t* char_info = &info.ch[ch];
	    char_info->width = bitmap.width;
	    char_info->height = bitmap.rows;
			//std::scoped_ptr<unsigned int, ArrayDeleter>(new unsigned int[2 * char_info->width * char_info->height]ArrayDeleter())
	    //Allocate memory for the bitmap data.
	    char_info->bitmap = new unsigned char[2 * char_info->width * char_info->height];
	    unsigned char* char_bmp = char_info->bitmap;

	    // Fill the bitmap data
	    for(int j=0; j <char_info->height;j++)
	    {
	        for(int i=0; i < char_info->width; i++)
	        {
	            char_bmp[2*(i+j*char_info->width)]= char_bmp[2*(i+j*char_info->width)+1] =
	                (i>=bitmap.width || j>=bitmap.rows) ?
	                0 : bitmap.buffer[i + bitmap.width*j];
	        }
	    }

	    // Accumulate the width of the bitmaps. Increase the rows if we reached the width of the texture
	    max_width += char_info->width;
	    if (max_width>2048-1)
	    {
	        max_width = char_info->width;
	        max_rows++;
	    }

	    // Determine what's the maximum height a given character might have
	    if (char_info->height>info.max_height)
	    {
	        info.max_height = char_info->height;
	    }

	    // Store information about this character. We will use this to print it.
	    char_info->row = max_rows;
	    char_info->left = bitmap_glyph->left;
	    char_info->top = bitmap_glyph->top;
	    char_info->advance = ft_face->glyph->advance.x >> 6;
	    char_info->x = max_width - char_info->width;
	}

	// Multiply the maximum height a character might have by the amount of rows and calculate the proper (power of two) height the texture must have.
	int texture_height = next_p2(info.max_height*(max_rows+1));

	// Create the temporary buffer for the texture
	unsigned char* texture_data = new unsigned char[/*m_texture_max_width*/2048*texture_height*2];

	// Fill the texture, set the vertex and uv array values and delete the bitmap
	for(unsigned int ch=0; ch<ft_face->num_glyphs; ch++)
	{
	    font::char_info_t* char_info = &info.ch[ch];

	    char_info->y = info.max_height*char_info->row;

	    fill_texture_data(ch, &info, 2048,  texture_data);

	    //(x,h)
	    int u = 0; int v = 1;
	    char_info->uv[0][u] = (float)char_info->x/2048;
	    char_info->uv[0][v] = (float)(char_info->y+char_info->height)/texture_height;

	    //(x,y)
	    char_info->uv[1][u] = (float)char_info->x/2048;               
	    char_info->uv[1][v] = (float)char_info->y/texture_height;

	    //(w,y)
	    char_info->uv[2][u] = (float)(char_info->x+char_info->width)/2048; 
	    char_info->uv[2][v] = (float)(float)char_info->y/texture_height;

	    //(w,h)
	    char_info->uv[3][u] = (float)(char_info->x+char_info->width)/2048; 
	    char_info->uv[3][v] = (float)(char_info->y+char_info->height)/texture_height;

	    delete [] info.ch[ch].bitmap;
	}

	// Create the texture and delete the temporary buffer
	{
		GLuint tid;
		glGenTextures(1, &tid);
		glBindTexture( GL_TEXTURE_2D, tid);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 2048, texture_height, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, texture_data);
		texture = eng_texture(tid, 2048, texture_height,2048, texture_height);
		printf("font: %d\n", tid);
	}
	delete [] texture_data;

	FT_Done_Face(ft_face);
	FT_Done_FreeType(library);
}

void font::fill_texture_data(unsigned int ch,
	font::font_info_t* font,
	unsigned int texture_width, unsigned char* texture_data)
{
    font::char_info_t* char_info = &font->ch[ch];
    unsigned char* char_bmp = char_info->bitmap;
 
    int bmp_pos = 0;
    int tex_pos = 0;
 
    int char_x = char_info->x;
    int char_y = char_info->y;
    int char_width = char_info->width;
    int char_height = char_info->height;
 
    for(int bmp_y=0; bmp_y<char_height; bmp_y++)
    {
        for(int bmp_x=0; bmp_x<char_width; bmp_x++)
        {
            bmp_pos = 2 * ( bmp_x + bmp_y * char_width);
            tex_pos = 2 * ( (char_x + bmp_x) + ( (char_y + bmp_y) * texture_width) );
 
            texture_data[tex_pos] = char_bmp[bmp_pos];
            texture_data[tex_pos+1] = char_bmp[bmp_pos+1];
        }
    }
}