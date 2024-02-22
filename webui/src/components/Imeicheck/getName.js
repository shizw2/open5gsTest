export const getStatusName = (status) => {
    switch (status) {
      case 0:
        return "白名单";
      case 1:
        return "黑名单";
      case 2:
        return "灰名单";
      default:
        return "白名单";
    }
  };

  export const getReasonName = (status) => {
    switch (status) {
      case 0:
        return "";
      case 1:
        return "丢失";
      case 2:
        return "被偷";
      case 3:
        return "被抢";
      case 4:
        return "非法厂商";
      case 5:
        return "未知原因";
      default:
        return "";
    }
  };