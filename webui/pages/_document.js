import Document, { Html,Head, Main, NextScript } from 'next/document'
import { ServerStyleSheet } from 'styled-components'
import properties from '../properties'; // 导入配置文件
const description = 'an implementation of 3GPP EPC(MME, SGW, PGW, HSS)'
/*
export default class MyDocument extends Document {
  render () {
    const sheet = new ServerStyleSheet()
    const main = sheet.collectStyles(<Main />)
    const styleTags = sheet.getStyleElement()
    return (
      <html>
        <Head>
          <title>{properties.base_title}</title>

          <meta charSet="utf-8" />
          <meta httpEquiv="X-UA-Compatible" content="IE=edge,chrome=1" />
          <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no" />

          <meta name="author" content="5GC" />
          <meta name="description" content={description} />

          <link rel='icon' href='/static/favicon.ico'/>
          <link rel="stylesheet" type="text/css" href="/static/css/bootstrap.min.css" />
          <link rel="stylesheet" type="text/css" href="/static/css/nprogress.css" />
          <link rel="stylesheet" type="text/css" href="/static/css/index.css" />
          {styleTags}
        </Head>
        <body>
          <div className='root'>
            {main}
          </div>
          <NextScript />
        </body>
      </html>
    )
  }
}
*/
export default class MyDocument extends Document {
  static async getInitialProps(ctx) {
    const sheet = new ServerStyleSheet()
    const originalRenderPage = ctx.renderPage

    try {
      ctx.renderPage = () =>
        originalRenderPage({
          enhanceApp: App => props => sheet.collectStyles(<App {...props} />),
        })

      const initialProps = await Document.getInitialProps(ctx)

      return {
        ...initialProps,
        styles: (
          <>
            {initialProps.styles}
            {sheet.getStyleElement()}
          </>
        ),
      }
    } finally {
     // sheet.seal()
    }
  }

  render() {
    return (
      <Html>
        <Head>
          {/* 移除以下行 */}
          {/* <title>{properties.base_title}</title> */}
          {/* viewport meta 标签 */}

          <meta charSet="utf-8" />
          <meta httpEquiv="X-UA-Compatible" content="IE=edge,chrome=1" />
          {/*<meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=no" />*/}

          <meta name="author" content="5GC" />
          <meta name="description" content={description} />

          <link rel='icon' href='/static/favicon.ico'/>
          <link rel="stylesheet" type="text/css" href="/static/css/bootstrap.min.css" />
          <link rel="stylesheet" type="text/css" href="/static/css/nprogress.css" />
          <link rel="stylesheet" type="text/css" href="/static/css/index.css" />
          {this.props.styles} {/* 使用 this.props.styles 渲染样式 */}
        </Head>
        <body>
          <Main />
          <NextScript />
        </body>
      </Html>
    )
  }
}
