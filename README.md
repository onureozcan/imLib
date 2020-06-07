# imLib
Poor man's graphics library.

It is not intended to be a production-grade project. I will use it in my hobby os to provide it a GUI and will try to make it portable as much as I can.

Currently supports brushes with different sizes and basic line drawing with antialiasing and alpha blending.

Also can parse glyph data from TTF files and draw them using bezier curves. No other features, however, supported yet.

Examples:

    image2d *image = image2d_new(800, 800);
    // draw a circular point of size 20 and hardness 1
    // hardness is defined within (0 to 1)
    image2d_draw_point(image, im_point(150, 50), im_color(255, 255, 255, 255), im_brush(20, 1, IM_BRUSH_SHAPE_CIRCULAR));
    // draw a line
    mage2d_draw_line(image, im_point(20, 200), im_point(500, 500), im_color(255, 255, 255, 255), im_brush(3, 1, IM_BRUSH_SHAPE_CIRCULAR));
    
    
![image](https://user-images.githubusercontent.com/21360651/83980061-4a101300-a91b-11ea-85df-b253113e445f.png) 

 


