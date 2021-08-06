# Custom SFX
If you want custom SFX, then...

  1. Go to your build folder and create a sub-folder named *custom* (for instance, *x64/Release/sfx/custom*).  
  2. In that folder, put all of your custom sound effects in \*.wav format.  
  3. Copy the file [_sfx-sample.json](_sfx-sample.json) to that folder and rename it to **_sfx.json**.  
  4. Open the file and list all of the sound effects you want to use and fill all key fields.  
  5. The sound list will be loaded next time you start ParsecSoda (no need to rebuild the application).

## _sfx.json keys explained:
| Key | What it does |
| :------- | :----- |
| tag | SFX command will use this to identify what sound to play (e.g.: !sfx tag). |
| cooldown | A timer in seconds that prevents spam. This number should be at least larger than the sound duration, as making it shorter may ocasionally crash the app if the users spam the SFX command. |
| path | The sound file location, this must match exactly the sound file name. |

## Download link
As alternative, I'll leave a download link to the SFX folder I currently use, with everything included so you just need to extract the 7z contents to the *sfx/custom* folder.

## [My custom SFX](https://mega.nz/file/MJwGEAAQ#C6MUO7ZT0Dz9PqYAOIN7b7JTrjXFVJGxNzL9P64F7KE)