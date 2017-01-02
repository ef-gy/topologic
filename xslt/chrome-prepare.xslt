<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
              xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
              xmlns:xhtml="http://www.w3.org/1999/xhtml"
              version="1.0">
  <xsl:output method="html" encoding="utf-8" indent="no" cdata-section-elements="xhtml:script" />

  <xsl:strip-space elements="*"/>

  <xsl:template match="/">
    <xsl:text disable-output-escaping='yes'>&lt;!DOCTYPE html></xsl:text>
    <xsl:apply-templates select="node()"/>
  </xsl:template>

  <xsl:template match="@*|node()">
    <xsl:copy>
      <xsl:apply-templates select="@*|node()" />
    </xsl:copy>
  </xsl:template>

  <xsl:template match="xhtml:script[@src='https://code.jquery.com/jquery-2.1.1.min.js']"/>
  <xsl:template match="xhtml:script[@src='https://code.jquery.com/mobile/1.4.4/jquery.mobile-1.4.4.min.js']"/>

  <xsl:template match="xhtml:script[@src='topologic-web.js']">
    <script src="topologic.js"/>
  </xsl:template>
</xsl:stylesheet>
