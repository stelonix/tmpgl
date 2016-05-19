#include "include/json.hpp"
#include "debug.h"
#include "fonts.h"
#include "generators.h"
#include <ft2build.h>
#include FT_FREETYPE_H 
#include <freetype/ftglyph.h>

extern FT_Library library;

string font::to_json()
{/*
	json j;
	for (	auto kvp = info.ch.begin();
			kvp != info.ch.end();
			kvp++)
	{
		json ch;
		ch[kvp.first] = kvp.second;
	}*/
		return "";
}

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
					"./Acme 5 Compressed Caps Outline Xtnd.ttf",
					0, &ft_face);
	if ( error == FT_Err_Unknown_File_Format )
	{
		printf("could not load font\n");
	}
	//FT_Set_Char_Size( ft_face, 18<< 6, 18 << 6, 96, 96);
	FT_Set_Pixel_Sizes(ft_face, 16, 16);
	//Initialize the char_info array
	//info.ch.resize(ft_face->num_glyphs);
	info.max_height = 0;

	// Variables to hold the size the texture must have to hold all the bitmaps
	int max_width = 0;
	int max_rows = 0;

	// Create all the characters
	int y = 0;
	int total_tried = 0;
	FT_UInt   gindex;
	for(auto ch = FT_Get_First_Char(ft_face, &gindex ); gindex!=0; ch = FT_Get_Next_Char( ft_face, ch, &gindex ))
	{
		char unicode_buf[10];
	    if(FT_Load_Glyph(ft_face, FT_Get_Char_Index( ft_face, ch ), FT_LOAD_NO_HINTING ))
	    {
	        throw std::runtime_error("FT_Load_Glyph failed");
	    };
	    //sprintf(unicode_buf, "\\u%c")
	    
	    FT_Glyph glyph;
	    if(FT_Get_Glyph( ft_face->glyph, &glyph ))
	    {
	        throw std::runtime_error("FT_Get_Glyph failed");
	    }
		y++;
		//printf("%d glyphs loaded\n", y);
	    //Convert the glyph to a bitmap.
	    FT_Glyph_To_Bitmap( &glyph, FT_RENDER_MODE_NORMAL, 0, 1 );
	    FT_BitmapGlyph bitmap_glyph = (FT_BitmapGlyph)glyph;
	    FT_Bitmap& bitmap = bitmap_glyph->bitmap;
	    //printf("bitmap->pixel_mode: %d\n", bitmap.pixel_mode);
	    
	    font::char_info_t* char_info = &info.ch[ch];
	    char_info->width = bitmap.width;
	    char_info->height = bitmap.rows;
	    //Allocate memory for the bitmap data.
	    char_info->bitmap = new unsigned char[4 * char_info->width * char_info->height];
	    unsigned char* char_bmp = char_info->bitmap;

	    // Fill the bitmap data
	    //dbgprint("Currently: %c\n", ch);
	    for(int j = 0; j < char_info->height; j++)
	    {
	    	//
	        for(int i=0; i < char_info->width; i++)
	        {
	            char_bmp[4*(i+j*char_info->width)] = bitmap.buffer[i + bitmap.width*j];
	            char_bmp[4*(i+j*char_info->width)+1] = bitmap.buffer[i + bitmap.width*j];
	            char_bmp[4*(i+j*char_info->width)+2] = bitmap.buffer[i + bitmap.width*j];
	            char_bmp[4*(i+j*char_info->width)+3] = bitmap.buffer[i + bitmap.width*j];
	            
	            //printf("%s", char_bmp[2*(i+j*char_info->width)+1]!=0?"!":"#");
	        }
	        //printf("\n");
	    }

	    // Accumulate the width of the bitmaps. Increase the rows if we reached the width of the texture
	    max_width += char_info->width;
	    if (max_width > 2048-1)
	    {
	        max_width = char_info->width;
	        max_rows++;
	    }

	    // Determine what's the maximum height a given character might have
	    if (char_info->height > info.max_height)
	    {
	        info.max_height = char_info->height;
	    }

	    char_info->row = max_rows;
	    char_info->left = bitmap_glyph->left;
	    char_info->top = bitmap_glyph->top;
	    char_info->advance = ft_face->glyph->advance.x >> 6;
	    char_info->x = max_width - char_info->width;
	    total_tried = ch;
	}
	printf("%d glyphs tried\n", total_tried);
	// Multiply the maximum height a character might have by the amount of rows and calculate the proper (power of two) height the texture must have.
	int texture_height = next_p2(info.max_height*(max_rows+2));

	// Create the temporary buffer for the texture
	unsigned char* texture_data = new unsigned char[/*m_texture_max_width*/2048*texture_height*2];

	// Fill the texture, set the vertex and uv array values and delete the bitmap
	int char_x, char_y;
	char_x = 0; char_y = 0;
	for(auto it = info.ch.begin(); it != info.ch.end(); it++)
	{
	    font::char_info_t* char_info = &((*it).second);

	    char_info->y = info.max_height*char_info->row;
	    auto ch = (*it).first;
	    if (char_x > 2048-1) {
	    	char_x = 0;
	    	char_y += info.max_height;
	    	break;
	    	printf("y desce: %d\n", char_y);
	    }
	    printf("%c stored at x = %d\n", ch, char_x);
	    fill_texture_data(ch, &info, 2048,  texture_data,char_x, char_y);
	    char_x += char_info->width;
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

		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, 2048, texture_height, 0, GL_BGRA, GL_UNSIGNED_BYTE, texture_data);
		texture = eng_texture(tid, 2048, texture_height,2048, texture_height);
		printf("font: %d\n", tid);
	}
	delete [] texture_data;

	FT_Done_Face(ft_face);
	FT_Done_FreeType(library);
}

void font::fill_texture_data(unsigned int ch,
	font::font_info_t* font,
	unsigned int texture_width, unsigned char* texture_data,
	int char_x, int char_y)
{
    font::char_info_t* char_info = &font->ch[ch];
    unsigned char* char_bmp = char_info->bitmap;
 
    int bmp_pos = 0;
    int tex_pos = 0;
 
     char_x = char_info->x;
     char_y = char_info->y;
    int char_width = char_info->width;
    int char_height = char_info->height;
 
    for(int bmp_y = 0; bmp_y < char_height; bmp_y++)
    {
        for(int bmp_x = 0; bmp_x < char_width; bmp_x++)
        {
            bmp_pos = 4 * ( bmp_x + bmp_y * char_width);
            tex_pos = 4 * ( (char_x + bmp_x) + ( (char_y + bmp_y) * texture_width) );
            texture_data[tex_pos] = char_bmp[bmp_pos];
            texture_data[tex_pos+1] = char_bmp[bmp_pos];
            texture_data[tex_pos+2] = char_bmp[bmp_pos];
            texture_data[tex_pos+3] = char_bmp[bmp_pos];
            //texture_data[tex_pos+1] = char_bmp[bmp_pos];
        }
    }
}

coord_grid text_buffer::set(string str, font* fnt)
{
	int size = str.size();
	int advance = 0;
	auto z = 4/100.0f;
	auto c = str.begin();
	auto tmp_end = str.end();

	font::char_info_t* ci;

	// Fill vertex data
	coord_grid vertval;
	for(; c != tmp_end; ++c)
	{
		printf("%c", *c);
	    ci = &fnt->info.ch[*c];
	    advance += ci->advance;

	    auto values = std::array<float, 18>(
		{
			ci->left+advance+0, 0 + (fnt->info.max_height-ci->top), z,
			ci->left+advance+ci->width, 0 + (fnt->info.max_height-ci->top), z,
			ci->left+advance+ci->width, ci->height + (fnt->info.max_height-ci->top), z,
			ci->left+advance+0, 0 + (fnt->info.max_height-ci->top), z,
			ci->left+advance+0, ci->height + (fnt->info.max_height-ci->top), z,
			ci->left+advance+ci->width, ci->height + (fnt->info.max_height-ci->top), z,
		});
		vertval.insert(vertval.end(), values.begin(), values.end());
	}

	// Fill UV data
	coord_grid texval;
	c = str.begin();
	for(; c != tmp_end; ++c)
	{
	    ci = &fnt->info.ch[*c];
		auto values = std::array<float, 12>(
		{
			ci->uv[1][0], ci->uv[1][1],
			ci->uv[2][0], ci->uv[2][1],
			ci->uv[3][0], ci->uv[3][1],
			ci->uv[1][0], ci->uv[1][1],
			ci->uv[0][0], ci->uv[0][1],
			ci->uv[3][0], ci->uv[3][1]	
		});
		texval.insert(texval.end(), values.begin(), values.end());
	}
	return gen::intercalate<3,2>(vertval, texval);
}