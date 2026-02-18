// ===========================
// 1pt: Simple Functions
// ===========================
// ai was used to help with the first draft//
// Increment the counter
function tickUp() {
    let counterElement = document.getElementById("counter");
    let currentValue = parseInt(counterElement.textContent);
    counterElement.textContent = currentValue + 1;
}

// Decrement the counter
function tickDown() {
    let counterElement = document.getElementById("counter");
    let currentValue = parseInt(counterElement.textContent);
    counterElement.textContent = currentValue - 1;
}

// ===========================
// 1pt: Simple For Loop
// ===========================

// Display all numbers from 0 up to and including the counter
function runForLoop() {
    let counterElement = document.getElementById("counter");
    let counterValue = parseInt(counterElement.textContent);
    let resultElement = document.getElementById("forLoopResult");
    
    let result = "";
    for (let i = 0; i <= counterValue; i++) {
        result += i + " ";
    }
    
    resultElement.textContent = result.trim();
}

// ===========================
// 1pt: Repetition with Condition
// ===========================

// Display all odd numbers from 1 to the counter
function showOddNumbers() {
    let counterElement = document.getElementById("counter");
    let counterValue = parseInt(counterElement.textContent);
    let resultElement = document.getElementById("oddNumberResult");
    
    let result = "";
    for (let i = 1; i <= counterValue; i++) {
        if (i % 2 !== 0) {  // Check if the number is odd
            result += i + " ";
        }
    }
    
    resultElement.textContent = result.trim();
}

// ===========================
// 1pt: Arrays
// ===========================

// Add every multiple of 5 up to the counter to an array in reverse order
// Print the array itself to console
function addMultiplesToArray() {
    let counterElement = document.getElementById("counter");
    let counterValue = parseInt(counterElement.textContent);
    
    let multiplesArray = [];
    
    // If counter is less than 5, print empty array
    if (counterValue < 5) {
        console.log(multiplesArray);
        return;
    }
    
    // Start from the highest multiple of 5 and work backwards (reverse order)
    for (let i = counterValue; i >= 5; i--) {
        if (i % 5 === 0) {
            multiplesArray.push(i);
        }
    }
    
    // Print the array itself to console
    console.log(multiplesArray);
}

// ===========================
// 2pts: Objects and Form Fields
// ===========================

// Create a car object from form fields and print it to console
function printCarObject() {
    let carType = document.getElementById("carType").value;
    let carMPG = document.getElementById("carMPG").value;
    let carColor = document.getElementById("carColor").value;
    
    let carObject = {
        cType: carType,
        cMPG: carMPG,
        cColor: carColor
    };
    
    console.log(carObject);
}

// ===========================
// 2pts: Objects and Form Fields pt. 2
// ===========================

// Load car data from predefined objects into the form
function loadCar(carNumber) {
    let carObject;
    
    // Select the appropriate car object based on the button clicked
    if (carNumber === 1) {
        carObject = carObject1;
    } else if (carNumber === 2) {
        carObject = carObject2;
    } else if (carNumber === 3) {
        carObject = carObject3;
    }
    
    // Load the data into the form fields
    document.getElementById("carType").value = carObject.cType;
    document.getElementById("carMPG").value = carObject.cMPG;
    document.getElementById("carColor").value = carObject.cColor;
}

// ===========================
// 2pt: Changing Styles
// ===========================

// Change the text color of the paragraph based on button clicked
function changeColor(colorNumber) {
    let paragraph = document.getElementById("styleParagraph");
    
    if (colorNumber === 1) {
        paragraph.style.color = "red";
    } else if (colorNumber === 2) {
        paragraph.style.color = "green";
    } else if (colorNumber === 3) {
        paragraph.style.color = "blue";
    }
}
