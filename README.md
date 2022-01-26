# CacheLineSize
By the Ajorpaz/Tullsen Group

# Viewing CSV Files
View the .csv file by uploading it to.
https://www.csvplot.com/

# Disabling HW Prefecture
`./wrmsr -p 0 0x1a0 0x60628e2289 `
`./rdmsr 0x1a0`

# Enabling HW Prefecture
`./wrmsr -p 0 0x1a0 0x60628e2089`
`./rdmsr 0x1a0`
