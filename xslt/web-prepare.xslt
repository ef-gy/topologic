<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
              xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
              xmlns:xhtml="http://www.w3.org/1999/xhtml"
              version="1.0">
  <xsl:output method="html" encoding="utf-8" indent="yes" />

  <xsl:strip-space elements="*"/>

  <xsl:param name="root"/>

  <xsl:template match="/">
    <xsl:text disable-output-escaping='yes'>&lt;!DOCTYPE html></xsl:text>
    <xsl:apply-templates select="node()"/>
  </xsl:template>

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()" />
    </xsl:copy>
  </xsl:template>

  <xsl:template match="xhtml:script[@src='topologic-web.js']">
    <xsl:apply-templates select="document(concat($root,'/topologic-web.js.xml'))/*"/>
  </xsl:template>

  <xsl:template match="xhtml:link[@href='topologic-web.css']">
    <xsl:apply-templates select="document(concat($root,'/topologic-web.css.xml'))/*"/>
  </xsl:template>
</xsl:stylesheet>
