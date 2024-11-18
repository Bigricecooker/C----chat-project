// 包含库
const path = require('path')
const grpc = require('@grpc/grpc-js')
const protoLoader = require('@grpc/proto-loader')

// 拼接，获取message.proto的路径
const PROTO_PATH = path.join(__dirname, 'message.proto')

// 同步加载并解析message.proto
const packageDefinition = protoLoader.loadSync(PROTO_PATH, { keepCase: true, longs: String, enums: String, defaults: true, oneofs: true })

// grpc解析
const protoDescriptor = grpc.loadPackageDefinition(packageDefinition)

// 获取message成员
const message_proto = protoDescriptor.message

// 导出，使其他js文件可以使用
module.exports = message_proto