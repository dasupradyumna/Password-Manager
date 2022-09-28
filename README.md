# Password Manager

## **Abstract**
A cross-platform C++-17 project for storing and retrieving user credentials for various online and
offline platforms. Below are the development phases -  
1. Console-based UI
2. Extending `Record`
3. File encryption & Data hashing
4. GUI support
5. Optimization (Data structures and Pipeline)
6. Multiple users and Network support

#### **Dependencies**
NCurses

---
### **Phase 1 : Console-based UI**
Every credential is stored as a record in the database at runtime. The database is loaded from (or
saved to) a file that is generated and used by the application. All user-supported operations are
performed on the runtime database in memory. Records are objects of the `Record` class.  
`[ 2 Sep 2022 - ongoing ]`
#### **Features TODO**
- ~~Implement application, screens and actions for moving between screens~~
- ~~Add behavior to "Delete" option in "Select" screen~~
- Add path to save directory as an option in "Settings" screen
- ~~Implement database, handle passing from "Select" screen to "Database" screen~~
- ~~Read from and write to the database (which is linked to a save file)~~
- ~~Move databases (core and display) to application, pass around with references~~
- Add a global storage for the application to share data between screens
- Abstract away actions and their behavior (currently mixed in with display)
- ~~Per-field input validation during input and overall entry check before saving~~
- Handle varying terminal size
- Number of entries per page in database screen / scrolling (settings)
- Consider separating "Delete" screen to derive from "Select" screen
- Different threads for display and user input
- Enable clipboard usage within the application

---
### **Phase 2 : Extending `Record`**
Adding more types of records to `Record` class for greater input flexibility to the user. A blank
record type also to be added, where the user can input *both* the key name and the value name for
the data key-value pairs, as they desire.

---
### **Phase 3 : File encryption & Data hashing**
Saving the runtime database to the storage file as plain text is extremely unsafe in terms of
security, since anyone with access to this file has knowledge of all saved credentials. The storage
file should be encrypted before saving and decrypted before loading. By itself, this is insufficient
because access to the public key can open up the encrypted file and leak the data. For this, we hash
the vital part of the credentials before encryption, possibly with added **salt** for more security.

---
### **Phase 4 : GUI support**
Convert the current TUI to a user-friendly GUI; with GLFW, ImGUI or Qt.

---
### **Phase 5 : Optimization (Data structures and Pipeline)**
Use Trie or Red-Black trees for the runtime database, to support efficient sorting, filtering and
labeling.

---
### **Phase 6 : Multiple users and Network support**
Add infrastructure for multiple user logins, and support for online cloud storage or remote access.

---
