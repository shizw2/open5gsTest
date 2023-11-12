const mongoose = require('mongoose');
const Schema = mongoose.Schema;
require('mongoose-long')(mongoose);

const OmmlogSchema = new Schema({

  schema_version: {
    $type: Number,
    default: 1  // Current Schema Version
  },

  opuser: [String],
  optype: [String],
  optcommand: [String],
  optfm: [String],
  opttime:[String]

}, { typeKey: '$type' });
let isOverwriting = false;
console.log(' write log://' +isOverwriting );
// 定义 pre 钩子
OmmlogSchema.pre('save', async function(next) {
  const maxDocuments = 10000; // 最大文档数量
  const collectionName = 'ommlogs'; // 集合名称
  console.log('> Ready write logs://' +isOverwriting );
  if (!isOverwriting) {
    // 获取当前集合中已有文档的数量
    const documentCount = await mongoose.model('Ommlog').countDocuments();

    if (documentCount >= maxDocuments) {
      // 如果已有文档数量达到最大值，查询最早的文档（限制返回的文档数量为 1）
      const oldestOmmlogs = await mongoose.model('Ommlog').find({ _id: { $ne: this._id } }).sort({ createdAt: 1 }).limit(maxDocuments/2);

      if (oldestOmmlogs.length > 0) {
        // 更新最早的文档
        isOverwriting = true; // 设置标志位为 true，表示正在进行循环覆盖操作
        await mongoose.model('Ommlog').deleteMany({ _id: { $in: oldestOmmlogs.map(ommlog => ommlog._id) } });
        isOverwriting = false; // 操作完成后，将标志位重置为 false
      }
    }
  }

  // 继续保存新文档
  next();
});

module.exports = mongoose.model('Ommlog', OmmlogSchema);
