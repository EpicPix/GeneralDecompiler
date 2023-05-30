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
    const stream = generateFile("arch/x86_64", "instructions_x86_64_include.h");
    stream.writeln(`uint8_t instr_byte = read_byte(data->bytes, &index, data->byte_count);`);
    for(let i = 0; i<instructions_x86_64.length; i++) {
        const instr = instructions_x86_64[i];
        const instr_byte = instr['enc']['b'];
        const instr_mask = instr['enc']['bm'] ?? 0xff;
        const modrm = instr['enc']['modrm'] ?? false;
        const rmreg_check = instr['enc']['rmreg'] ?? false;

        stream.writeln(`if((instr_byte & 0x${instr_mask.toString(16).padStart(2, '0')}) == 0x${instr_byte.toString(16).padStart(2, '0')}) {`);

        if(modrm) {
            stream.writeln("  const uint8_t modrm_byte = read_byte(data->bytes, &index, data->byte_count);");
        }

        if(rmreg_check) {
            stream.writeln("  const uint8_t modrm_reg = (modrm_byte >> 3) & 0x07;");
            for(const [k,v] of Object.entries(instr['rmreg'])) {
                stream.writeln(`if(modrm_reg == ${k}) {`);
                generateInstr(v);
                stream.writeln(`}`);
            }
        }else {
            generateInstr(instr);
        }

        function generateInstr(instr) {
            const dat_types = [];
            let d = 0;
            for(const dat of instr['dat']) {
                const datv = dat.split("/");
                dat_types.push(datv[0]);
                if(datv[0] === "imm") {
                    const type_conversions = { "8": "byte", "16": "word_le", "32": "int_le", "64": "long_le" };
                    const size_type = type_conversions[datv[1]];
                    stream.writeln(`  const uint64_t dyn${d} = read_${size_type}(data->bytes, &index, data->byte_count);`);
                }else if(datv[0] === "reg") {
                    if(datv[1] === "modrm") {
                        stream.writeln(`  const uint64_t dyn${d} = (modrm_byte >> 3) & 0x07;`);
                    }else if(datv[1] === "op") {
                        stream.writeln(`  const uint64_t dyn${d} = instr_byte ${datv[2]};`);
                    }
                }else if(datv[0] === "rm") {
                    if(datv[1] === "modrm") {
                        stream.writeln(`  const struct ir_instruction_high_location dyn${d} = arch_modrm_location(modrm_byte >> 6, modrm_byte & 0x7, ir_type_${datv[2]});`)
                    }
                }

                d++;
            }

            const gen = instr['gen'];

            function generateInstructionData(data) {
                const d = data.split("/");
                if(d[0] === "dyn") {
                    const dx = Number.parseInt(d[1]);
                    const x = dat_types[dx];
                    if(x !== "rm") {
                        stream.write(`{ .type = ir_type_${gen['s']}, .location_type = ir_instruction_high_location_type_`);
                        if (x === "reg") {
                            stream.write(`register, .data = { .reg = dyn${dx} }`);
                        } else if (x === "imm") {
                            stream.write(`immediate, .data = { .imm = dyn${dx} }`);
                        }
                        stream.write("}");
                    }else {
                        stream.write(`dyn${dx}`);
                    }
                }else if(d[0] === "imm") {
                    stream.write(`{ .type = ir_type_${gen['s']}, .location_type = ir_instruction_high_location_type_immediate, .data = { .imm = ${d[1]} } }`);
                }
            }

            stream.writeln(`  instructions = ir_instruction_add_instruction_high(instructions, 1024, (struct ir_instruction_high) {`);
            stream.writeln(`    .type = ir_instruction_high_type_${gen['t']},`);
            stream.writeln(`    .data = {`);
            stream.writeln(`      .${gen['op']['dst'] ? "o" : ""}${(gen['op']['src'] || gen['op']['src_a']) ? "i" : ""}${gen['op']['src_b'] ? "i" : ""} = {`);
            if(gen['op']['dst']) { stream.write(".output = "); generateInstructionData(gen['op']['dst']); stream.writeln(","); }
            if(gen['op']['src']) { stream.write(".input = "); generateInstructionData(gen['op']['src']); stream.writeln(","); }
            if(gen['op']['src_a']) { stream.write(".inputa = "); generateInstructionData(gen['op']['src_a']); stream.writeln(","); }
            if(gen['op']['src_b']) { stream.write(".inputb = "); generateInstructionData(gen['op']['src_b']); stream.writeln(","); }
            stream.writeln(`      }`);
            stream.writeln(`    }`);
            stream.writeln("  });");
        }

        if(i === instructions_x86_64.length - 1) {
            stream.writeln("}");
        }else {
            stream.write("} else ");
        }
    }
    stream.close();
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
    return stream;
}

runSourceGenerator();