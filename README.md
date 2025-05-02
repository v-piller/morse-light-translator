# 🌸Morse Code Translator💕


## Sensor: Light Sensor and Flashlight

Sensor we are using: https://shop.m5stack.com/products/light-sensor-unit?srsltid=AfmBOorAT_KwV2GzEu7Np9zr26Jv7H7u_yf2j1E82jheaLuKGSv_uy9e
LED light we are using: https://docs.m5stack.com/en/unit/FlashLight  /  https://www.bastelgarage.ch/m5stack-led-flashlight-unit-1-2643


## Idea:

Make a server/web page where we can interact with the sensor (send messages that will be translated in morse code and receive messages in morse code that will be translated in a readable message). Two computers can interact with each other (send and receive).

We would use a LED light? to send the morse code from Computer 1 that will be sensed by Computer 2's sensor. The sesnor will have to understand the dot and dashes, and that information will be translated to a readable message and shown in the server/web page. Then, Computer 2 would be able to write a response and send it back to Computer 1, so that it can sens, understand, translate and show. Basically, a comunication system.

What do we do with the microprocessor?
We would use it as the translator.


---

## BLUEPRINT / MODEL OF THE PROJECT (MAIN IDEA)

### 💻 WHAT DOES THE MICROCONTROLLER DO?

1. It **receives light signal info**, converts it into **binary**, then into **letters**.  
   - Sends the **message word by word**.  
   - Stores the words in a data structure (as individual strings or full message?).  
   - Possibility to implement a **timestamped history** (like a mini chat log?)

2. It can also do the **opposite direction**:  
   - Receive a message as text,  
   - Translate it to **Morse code**,  
   - Send the **light signal** accordingly through the light unit.

---

### 🌐 WEBPAGE AND SERVER

- The **webpage** sends a text string to the **server**.
- The **server** forwards it to the **microcontroller** and vice versa.
- The **server runs locally** on the computer and is connected to the webpage.

---

### 📚 LIBRARY

We found a **Morse code translation library** — it works, but:

- ❌ No automatic handling of **full text translation**
- ❌ No space management between **words**
- ❌ No **parsing of full Morse sequences**

So we **added all of that ourselves** ✨

---

### 🔄 HOW DO WE CONVERT & TRANSLATE?

We translate step by step:

- Light info  
- ➡️ Binary  
- ➡️ Dots and dashes (Morse)  
- ➡️ Letters  
- *(And vice-versa)*

### 🧃 Light Signal Interpretation

```plaintext
light duration → binary

1         → dot  
111       → dash  
0         → symbol space (between dot & dash)  
000       → letter space  
0000000   → word space  

Binary → Morse

0000000  → /   (word separator)  
. or //  →     (sentence separator)

Morse → letters

• We use a library that translates individual Morse letters  
• Full message = built one letter at a time 
```

### 💡 DIFFERENT IDEAS TO EXPLORE

#### ✨ Notifications

- Use an **event-driven system**  
    → Detect when a message arrives and trigger a **notification**  
    (pop-up, sound, cute visual alert? 🔔🌸)
    

### ✂️ **When We Have the Morse Message...**

#### 📚 Sentence Parser

- Detects sentence endings (`.` or `//`)
    
- Stores them in a **structured format** (like an array or list)
    

---

#### 💌 Word Parser

- Splits sentences into words (`/`)
    
- Stores each word in a **word-level data structure**
    

---

### 🧵 Two Options for Letter Translation

#### Option 1: Letter-by-letter 💋

- Split each Morse word into **individual letters**
    
- Translate each letter **one at a time**
    
- Append letters to a string to form the full word ✨
    

#### Option 2: Whole Word Decode 🌈

- Take a full Morse word
    
- Loop through the characters
    
- Translate and combine into a single word string 💕
    

   
