# 📱 Update Your Shake Cube from Your Phone - Complete Guide

## 🎯 Overview

Your Shake Cube now supports **100% phone-only updates** using:
1. **GitHub Actions** - Automatically builds firmware in the cloud
2. **Web-based OTA** - Upload firmware from any phone browser

---

## 🚀 One-Time Setup (Need Computer)

### Step 1: Push to GitHub

1. Open terminal in your project folder
2. Run these commands:

```bash
git init
git add .
git commit -m "Initial commit - Shake Cube with phone OTA"
git branch -M main
git remote add origin https://github.com/YOUR_USERNAME/ShakeCube.git
git push -u origin main
```

**Note:** Replace `YOUR_USERNAME/ShakeCube` with your actual GitHub repository URL.

### Step 2: Verify GitHub Actions

1. Go to your GitHub repository
2. Click "Actions" tab
3. You should see a build running or completed
4. Once complete, click on the build → "Artifacts" section
5. Download the `.bin` file to verify it works

---

## 📲 Daily Use: Update from Phone Only

### Step 1: Edit Code on Phone

**Option A: GitHub Mobile App** (Recommended)
1. Install "GitHub" app from App Store/Play Store
2. Sign in to your account
3. Navigate to your ShakeCube repository
4. Tap on `src/main.cpp`
5. Tap the pencil icon (top right) to edit
6. Make your changes
7. Scroll down → Add commit message → "Commit changes"

**Option B: GitHub Website**
1. Open browser on phone
2. Go to `github.com` and sign in
3. Navigate to your repository
4. Tap `src/main.cpp`
5. Tap the pencil icon to edit
6. Make changes
7. Commit directly to main branch

### Step 2: Wait for Build (2-3 minutes)

1. Go to "Actions" tab in your repository
2. You'll see a new workflow running
3. Wait for the green checkmark ✅
4. Tap on the completed workflow
5. Scroll down to "Artifacts" section
6. Download `shakecube-firmware` to your phone

### Step 3: Extract the .bin File

- **Android:** Use "Files" app or any unzip app
- **iOS:** Tap the zip file → "Extract All"
- You'll get a file like: `shakecube-20241116-123456-abc1234.bin`

### Step 4: Upload to Your Cube

1. **Turn on your Shake Cube**
   - Wait for boot animation (about 10 seconds)
   
2. **Connect to Cube WiFi**
   - Open WiFi settings on your phone
   - Connect to: `ShakeCube-Config`
   - Password: `shakecube123`

3. **Open Update Page**
   - Open any browser on your phone
   - Go to: `http://192.168.4.1/update`
   - You'll see the OTA update page

4. **Upload Firmware**
   - Tap "Choose File" / "Browse"
   - Select the `.bin` file you downloaded
   - Tap "Update"
   - Wait for upload (about 30 seconds)

5. **Done!**
   - Cube will reboot automatically
   - You'll see the boot animation
   - Your changes are now running!

---

## 🎨 Example: Change Animation Duration from Phone

### Edit on GitHub Mobile:

1. Open `src/main.cpp`
2. Find line ~300 in `playRandomAnimation()`:
```cpp
unsigned long duration = random(5000, 10001); // 5-10 seconds
```
3. Change to:
```cpp
unsigned long duration = random(3000, 6001); // 3-6 seconds (faster!)
```
4. Commit changes
5. Wait for build → Download → Upload to cube!

---

## 💡 Pro Tips

### Faster Workflow
- Keep the GitHub repository page bookmarked on your phone
- Keep the firmware download folder easily accessible
- Once connected to cube WiFi, bookmark `http://192.168.4.1/update`

### Multiple Cubes
- Each cube creates its own WiFi network
- You can name them differently by changing line 342 in `main.cpp`:
```cpp
WiFi.softAP("ShakeCube-Config", "shakecube123");
```
Change to:
```cpp
WiFi.softAP("ShakeCube-Living-Room", "shakecube123");
```

### Offline Editing
- Use GitHub's draft feature to prepare changes offline
- Commit when you have internet to trigger the build

### Download Firmware in Advance
- GitHub stores artifacts for 90 days
- You can download multiple builds and keep them on your phone
- Upload any version to your cube later

---

## 🔧 Troubleshooting

### "Can't connect to ShakeCube-Config"
- Restart the cube
- Wait 10-15 seconds for it to fully boot
- Check if you see the network in WiFi settings

### "Can't access 192.168.4.1/update"
- Make sure you're connected to `ShakeCube-Config` WiFi
- Some phones keep using cellular data - disable it temporarily
- Try `http://192.168.4.1/update` (not https)

### "Upload failed"
- Make sure you extracted the .bin file from the zip
- File should be about 300-400 KB in size
- Try rebooting the cube and uploading again

### "GitHub Actions build failed"
- Check the Actions tab for error messages
- Usually a syntax error in the code
- Fix the error and commit again

### "Where's my downloaded firmware?"
- **Android:** Usually in `Downloads` folder
- **iOS:** Files app → Downloads
- Look for files starting with `shakecube-`

---

## 🎉 You're All Set!

You can now:
- ✅ Edit code from anywhere using just your phone
- ✅ Build firmware automatically in the cloud (free!)
- ✅ Update your cube wirelessly without a computer
- ✅ Take your cube anywhere - it works without home WiFi

**No computer needed ever again!** 🚀

---

## 📚 What You Changed

Your cube now has these features:

1. **Always creates its own WiFi hotspot** (`ShakeCube-Config`)
2. **Works anywhere** - no dependency on home WiFi
3. **Web-based OTA updates** - upload from any browser
4. **Auto-builds on GitHub** - commit code, get firmware automatically

---

## 🔐 Security Notes

- The WiFi password is hardcoded in your code (line 9)
- The Access Point password is `shakecube123`
- Consider changing these if security is a concern
- The OTA update page has no authentication by default

---

**Need help?** Check the GitHub Issues page or refer to this guide!

