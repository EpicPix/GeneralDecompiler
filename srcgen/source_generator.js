const fs = require("fs");
const path = require("path");

const args = process.argv.slice(2);

if(args.length < 2) {
    console.log("Invalid arguments, expected in and out folders");
    exit(1);
}

const input = path.resolve(args[0]);
const output = path.resolve(args[1]);

function runSourceGenerator() {
    const instructions_x86_64 = readFile("instructions_x86_64");
    // generateFileWithData(".", "test.c", ``);
}

function readFile(name) {
    return JSON.parse(fs.readFileSync(input + "/" + name + ".json"));
}

function generateFileWithData(dir, name, data) {
    generateFile(dir, name).writeln(data).close();
}

function generateFile(dir, name) {
    const fdir = output + "/" + dir + "/generated/";
    fs.mkdirSync(fdir, { recursive: true });
    const fname = fdir + name;
    const stream = fs.createWriteStream(fname);
    stream.writeln = function(data) {
        stream.write(data.toString());
        stream.write("\n");
        return stream;
    }
    stream.writeln("/* Auto generated */\n");
    stream.writeln("#include <stdint.h>");
    stream.writeln("#include <stdbool.h>");
    return stream;
}

runSourceGenerator();