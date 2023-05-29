const fs = require("fs");
const path = require("path");

const args = process.argv.slice(2);

if(args.length < 2) {
    console.log("Invalid arguments, expected in and out folders");
    exit(1);
}

const input = args[0];
const output = path.resolve(args[1]);

function runSourceGenerator() {
    const test = generateFile(".", "test");

    test.close();
}

function generateFile(dir, name) {
    const fdir = output + "/" + dir + "/generated/";
    fs.mkdirSync(fdir, { recursive: true });
    const fname = fdir + name + ".c";
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