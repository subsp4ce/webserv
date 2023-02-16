console.log("Hello, JavaScript!");

async function copyToClipboard(text) {
    await navigator.clipboard.writeText(text);
    alert("Copied to clipboard")
}
