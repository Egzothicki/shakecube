# 📱 Update Your Shake Cube from Your Phone

Complete guide to updating firmware using only your phone!

## 🎯 Overview

- **Edit code** on GitHub (mobile app or browser)
- **Auto-build** firmware in the cloud (GitHub Actions)
- **Upload wirelessly** to your cube from any phone

**No computer needed!** ✨

---

## 🚀 One-Time Setup (Requires Computer)

### Push to GitHub

```bash
git init
git add .
git commit -m "Initial commit - Shake Cube"
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/ShakeCube.git
git push -u origin main
```

**Note:** Replace `YOUR_USERNAME/ShakeCube` with your repo URL.

### Verify Build Works

1. Go to repo → **Actions** tab
2. Wait for green checkmark ✅
3. Click build → **Artifacts** section
4. Download `.bin` file to test

---

## 📲 Daily Use: Phone-Only Updates

### Step 1: Edit Code 📝

**Option A: GitHub Mobile App** (Recommended)
1. Install "GitHub" app
2. Open your ShakeCube repository
3. Tap `src/main.cpp` → Edit (pencil icon)
4. Make changes → Commit

**Option B: Mobile Browser**
1. Go to `github.com`
2. Navigate to your repository
3. Open `src/main.cpp` → Edit
4. Make changes → Commit

### Step 2: Download Firmware 📥

1. Go to **Actions** tab
2. Wait for build to complete (2-3 min)
3. Tap completed workflow
4. Download **artifact** (zip file)
5. Extract the `.bin` file

### Step 3: Upload to Cube 📤

1. **Power on** your Shake Cube
2. **Connect to WiFi:**
   - Network: `ShakeCube-Config`
   - Password: `shakecube123`
3. **Open browser** → `http://192.168.4.1/update`
4. **Choose** the `.bin` file
5. **Tap "Update"**
6. Wait ~30 seconds - Done! ✅

---

## 🎨 Common Edits

### Change Animation Duration

**File:** `src/main.cpp` **Line:** ~324

```cpp
// Change from:
unsigned long duration = random(5000, 10001);  // 5-10 seconds

// To:
unsigned long duration = random(3000, 6001);   // 3-6 seconds
```

### Change LED Brightness

**File:** `src/main.cpp` **Line:** ~37

```cpp
// Change from:
const int LED_BRIGHTNESS = 8;    // 0-15

// To:
const int LED_BRIGHTNESS = 15;   // Maximum brightness!
```

### Change Shake Sensitivity

**File:** `src/main.cpp` **Line:** ~38

```cpp
// Change from:
const int SHAKE_DEBOUNCE = 1000; // Wait 1 second between shakes

// To:
const int SHAKE_DEBOUNCE = 500;  // Wait 0.5 seconds (more responsive)
```

### Rename WiFi Network

**File:** `src/main.cpp` **Lines:** ~42-43

```cpp
// Change from:
const char* AP_NAME = "ShakeCube-Config";
const char* AP_PASSWORD = "shakecube123";

// To:
const char* AP_NAME = "ShakeCube-Living-Room";
const char* AP_PASSWORD = "mypassword";
```

---

## 💡 Pro Tips

### Speed Up Your Workflow
- Bookmark your GitHub repo on your phone
- Bookmark `http://192.168.4.1/update`
- Keep firmware downloads organized

### Multiple Cubes?
- Change the WiFi name (see above)
- Each cube gets its own network
- Update them independently

### Save Firmware Versions
- GitHub keeps artifacts for 90 days
- Download and save multiple versions
- Roll back if needed

---

## 🔧 Troubleshooting

| Problem | Solution |
|---------|----------|
| Can't see `ShakeCube-Config` WiFi | Restart cube, wait 15 seconds |
| Can't access update page | Make sure you're connected to cube WiFi, disable cellular data |
| Upload fails | Extract `.bin` from zip, check file size (~300-400 KB) |
| Build fails on GitHub | Check Actions tab for errors (usually syntax error) |
| Where's my download? | Android: Downloads folder, iOS: Files app → Downloads |

### Still Not Working?

1. **Restart the cube**
2. **Check WiFi connection** (should show connected to ShakeCube-Config)
3. **Try from computer** using same method
4. **Check GitHub Actions** logs for build errors

---

## 📚 Technical Details

### What Happens Behind the Scenes?

1. **You commit code** → Triggers GitHub Action
2. **GitHub builds firmware** → Creates `.bin` file
3. **You download** → Get compiled firmware
4. **Upload to cube** → OTA bootloader flashes ESP8266
5. **Cube reboots** → Runs new code!

### Security Notes

- WiFi password is in your code (line ~43)
- No authentication on update page
- Consider changing default password
- Only share WiFi credentials with trusted users

### Files Explained

- **main.cpp** - Your code
- **platformio.ini** - Build settings
- **.bin file** - Compiled firmware (uploadable)
- **GitHub Actions** - Automatic build system

---

## 🎉 You're All Set!

Now you can:
- ✅ Edit code anywhere with just your phone
- ✅ Build firmware for free in the cloud  
- ✅ Update your cube wirelessly
- ✅ No computer required!

**Happy hacking!** 🚀

---

## 📖 Quick Reference Card

**WiFi:** ShakeCube-Config / shakecube123  
**Update URL:** http://192.168.4.1/update  
**Edit Code:** github.com (mobile)  
**Build Time:** 2-3 minutes  
**Upload Time:** ~30 seconds

---

**Questions?** Open an issue on GitHub or check the troubleshooting section above!
