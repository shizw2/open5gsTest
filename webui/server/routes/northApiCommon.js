/**
 * 统一错误码定义
 */
const errorCodes = {
    SUCCESS: 0,
    COMMON_ERROR: 1001,
    FILE_ERROR: 1002,// 文件操作错误
    PARAMETER_ERROR: 1003,// 参数错误
    // 其他自定义错误码...
  };

// 统一错误处理
function handleError(res, statusCode = 500, errorCode = -1,  msg, target = '', detail = {}) {
    const error = {
      code: errorCode, // 错误码，必须
      message: msg, // 描述信息，必须
      detail: {
        desc: detail.desc || '', // 细节描述，必须
      }
    };
  
    // 如果提供了target，则添加到错误对象中
    if (target) {
      error.target = target;
    }

    // 如果提供了无效参数的详细信息，则添加到错误对象中
    if (detail.invalidParam) {
      error.detail.invalidParam = {
        param: detail.invalidParam.param || '', // 无效参数值，当invalidParam存在时，param是必须
      };
      // 如果提供了无效参数的原因，则添加到错误对象中
      if (detail.invalidParam.reason) {
        error.detail.invalidParam.reason = detail.invalidParam.reason;
      }
    }
  
    res.status(statusCode).json({
      result: "FAIL",
      error: error
    });
  }

  module.exports = { handleError, errorCodes };