=======================================================================
=   Memory Card Annihilator v2.0 coded by ffgriever, gfx by Berion    =
=======================================================================

1. What is MCAnnihilator?

      Memory Card Annihilator is a tool created back in 2007. Its main
   purpose is formatting/unformatting memory cards (both PS2 and PSX)
   as well as managing memory card images (creating images of physical
   cards and writing images onto physical cards). It can be used to
   restore cards broken by other formatters back to normal (eg. when
   64MB card has been formatted to 8MB by MCKiller and similar tools).

      It has quite nice bad block handling, so even worn cards should
   work fine after formatting (unless it's the very first block that
   got screwed - because of the way mcman handles the cards when
   recognizing the file system, I don't think this kind of error
   could be solved in a different way than hardware block remapping).

      The previous version, although worked really nice (got quite
   a lot of success stories), had two big drawbacks:

      a) was all in Polish language,
      b) display mode was fixed to PAL.

      Now in 2010 I created my own "GUI Framework". It's pretty
   generic, so could be probably used on any platform, but I created
   dedicated renderers, input systems, texture managers, io systems for
   PC Windows (rendering using OpenGL) and PS2. I finally got bored
   of the unit tests (well... for some systems "unit" tests were not
   really even possible, he, he). So along with Berion we decided to
   re-release changed MCAnnihilator. It uses the new framework to do
   all the GUI. The core part is mostly the same as the one from 2007
   (just some fixes done). So it now supports multiple languages,
   can display in PAL/NTSC/VGA. It doesn't make a full use of the
   framework (just some simple windows, effects, text printing,
   input, etc.), but it was a great "real life" test for it. I was
   able to find many bugs (especially in PS2 Vram Manager). I've
   also noticed the fwork needed some additional stuff. Nevertheless,
   although it's just a test, it is a fully fledged application.

      So what has been actually changed? A lot of things, but here
   is a short list of the most important changes:

      a) Berion has retouched and refreshed (or changed) a lot of the
   (beautiful to begin with) graphics.
      b) The app supports now language files (utf8), so the translation
   should be quite easy.
      c) Fixed some errors in the iop code.
      d) Some eyecandies added.
      e) New display modes supported (PAL/NTSC/VGA).
      f) Images can be written to or read from various devices.
      g) There is a simple file explorer and virtual keyboard,
   so you can select an exact placement and name of file you want
   to write/read.
      h) Everything should work a little bit faster.
      i) Some issues with mass devices fixed.
      j) MC information screen added.
      k) Some additional warnings and checks.
      l) Now it is possible to select size which you want to format
   your PS2 memory card to (with a step of 1MByte, using L1/R1 or by
   nearest power of two by using left/right).
      m) Some windows moved, changed, removed.
      n) Some behavior changes (well, the GUI itself has been written
   anew using the framework, the same applies to action scripting).
      o) Many more small changes not worth mentioning.

2. Basic usage.

      The video mode needed should be detected automatically (for
   PAL/NTSC - depending on console). But if it fails for some reason,
   you can always force particular mode by keeping a button pressed
   during application startup. The options are as follows:

      a) PAL - press dpad RIGHT.
      b) NTSC - press dpad LEFT.
      c) VGA (640x480) - press dpad UP.

      All the gfx has been made with PAL (640x512 framebuffer) mode
   in mind, so for NTSC (640x448 framebuffer) and VGA (640x480
   framebuffer), everything is downscaled to match the screen
   and preserve on-screen aspect ratio (not the framebuffer pixel
   ratio). This makes the app to look a little bit worse in NTSC
   and VGA. One possibility would be to move the screen up and not
   scale it. But that caused top and bottom to be cut off... And the
   on-screen aspect ratio would change (so everything would look like
   it was stretched vertically up).

      The possibility to exit application and launch another elf
   (browsing) has been added just before the release... Take it as
   an experimental feature. It should work fine, though.

      The keys are pretty straightforward. Some are not that obvious,
   but whenever needed, a hint is displayed to make sure the user
   knows what to do.

      - dpad up/down: change menu item selection, change file selection
   in explorer.
      - dpad left/right: change horizontal menu item selection
   (yes/no), change submenu item selection (fast/full formatting),
   change memory card size in the size selection window to the nearest
   of 8/16/32/64/128MB.
      - L1/R1: change memory card size in the size selection window
   by the unit of 1MB, jump to the next/previous page in file explorer.
      - L2/R2: enable/disable file mask in file explorer (on by
   default).
      - START: display virtual keyboard in file explorer, display
   "thanks/about" window.
      - SELECT: when in main menu (card selection screen), display
   "exit to" dialog and browser to select elf file to exit to, display
   memory card info screen when in memory card screen (there is also
   an additional menu item that does exact same thing).
      - X: confirm.
      - TRIANGLE: cancel/return.

      I decided to put the key mapping for virtual keyboard in
   separate paragraph to make things easier for everyone. You can
   open the virtual keyboard when using file explorer in write mode
   by pressing START button.

      - left analog stick: move cursor.
      - dpad: move cursor (you can switch analog mode on/off by using
   "mode" button).
      - X: "press" the highlighted key.
      - SQUARE: space (can also be pressed on screen).
      - CIRCLE: backspace (can also be pressed on screen).
      - R1: shift (active only when button is pressed, can also be
   pressed on screen).
      - L1: caps lock on/off (can also be pressed on screen).
      - START: enter (confirms filename, exits keyboard, can also be
   pressed on screen).
      - TRIANGLE: exit keyboard without selecting filename.

3. Menu items description.

FORMAT

      It basically does what it says. The iop code will clear all the
   card blocks, check the card for bad blocks (multiple times to make
   sure) and save bad blocks in an array, then it will write the new
   filesystem. There are two options: "Fast" and "Full". Fast should
   be ok for most cases. Full makes additional bad block checks and
   writes all the card pages (necessary in some cases... takes few
   times as long as fast). Fast is the default (you can just leave
   it... unless you're experiencing some problems).

UNFORMAT

      Clears all blocks. It leaves the card in a state it should be
   when you bought it (totally clear, no filesystem on card). Then
   you can use PS2 Browser, in-game tools or whatever you like to
   format it.

CREATE MC IMAGE

      Creates an image of memory card (or a portion of it) and stores
   it in a selected place. Creates a raw memory card image (without ECC
   after every page, so you can mount it in uLE or use it with mcsio
   emulator or OPL). Stores MCI descriptor file for PS2 memory cards.

RESTORE MC IMAGE

      Restores an image of memory card (writes the selected image
   to the physicall memory card). Only raw memory card images are
   supported - no ECC block after every page (no worries, MCA will
   calculate ECC on the fly if needed). If the image is shorter than
   the card capacity, an error will be displayed (all the image data
   will be written, though). IMPORTANT NOTICE: MCA WILL NOT CHECK
   FOR BAD BLOCKS WHEN RESTORING THE IMAGE. THAT WOULD REQUIRE TO
   CHANGE THE IMAGE DATA, WCHICH IS UNACCEPTABLE (it's fine only if
   you're trying to store standard MC filesystem data... which is most
   likely the only thing you will do, but for the sake of versatility,
   it has to be left this way.)!

MEMORY CARD INFO

      Displays some basic information regarding the card in currently
   selected slot. There is a "shortcut" to this option - just press
   "SELECT" button.

4. Translations (language files).

      MCA searches for "lang.lng" file in the same directory it
   was launched from. It is done before IOP reset is performed,
   so it expects to have all the needed modules for the selected
   device loaded.

      The language file parser supports UTF8, so if your language
   contains non ASCII characters, save the file in UTF8 format. Default
   fonsets support all the latin, latin suplementary (diacritic
   characters used in Polish, Spanish, Turkish, Czech, Portuguese,
   etc.), cyryllic (Russian and other languages), additional
   punctuation and some additional glyps (mostly chars like (R),
   1/4 and such).

      The language file parser has no real grammar defined, so it is
   pretty forgiving when it comes to mistakes (whitespaces, order, heck
   you can even place the string first, then the identifier and the
   equal sign at the very end and it will still be parsed properly. It
   has some restrictions, though. The IDENTIFIER is CASE SENSITIVE. The
   string has to be placed in straight double quotation marks. The
   entry CANNOT span across multiple lines. The following escaped
   characters can be used: \n, \". The general format is as follows:

-------file lang.lng
#line comment
//line comment
LNG_IDENTIFIER1="String to be printed.\nNext line."
LNG_IDENTIFIER2="\nThis string starts with a newline."
LNG_IDENTIFIER3="This one contains \"quotes\" inside."
--------------------

5. Disclaimer.

      This application is free and provided "as is", without warranty
   of any kind, either expressed or implied. The authors cannot be
   held responsible for any damage to the hardware or data loss caused
   by application usage and missusage.


~ ffgriever & berion 09.09.2010
