<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="2.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:col="http://www.collada.org/2005/11/COLLADASchema">

  <xsl:strip-space elements="doc chapter section"/>
  <xsl:output method="text" indent="yes" standalone="yes" 
     encoding="iso-8859-1"
     omit-xml-declaration="yes"/>

 <!-- Effect Templates -->
  <xsl:template match="col:COLLADA/col:library_effects/col:effect">
    <xsl:text>:: --------------------------------------------------------------------------&#xA;</xsl:text>
    <xsl:text>:: This file has been generated from a COLLADA FX File. Do not edit manually.&#xA;</xsl:text>
    <xsl:text>:: --------------------------------------------------------------------------&#xA;</xsl:text>
    <xsl:for-each select="col:profile_CG[@platform='PS3']">
        <xsl:for-each select="col:technique/col:pass">
          <xsl:variable name="VertexShader" select="col:shader[@stage='VERTEX']"/>
          <xsl:variable name="FragmentShader" select="col:shader[@stage='FRAGMENT']"/>

          <xsl:call-template name="ShaderProgram">
            <xsl:with-param name="Profile">sce_vp_rsx</xsl:with-param>
            <xsl:with-param name="Ext">.vpo</xsl:with-param>
            <xsl:with-param name="Shader" select="$VertexShader"/>
            <xsl:with-param name="cgcstripOptions">-param </xsl:with-param>
          </xsl:call-template>
          <xsl:call-template name="ShaderProgram">
            <xsl:with-param name="Profile">sce_fp_rsx</xsl:with-param>
            <xsl:with-param name="Ext">.fpo</xsl:with-param>
            <xsl:with-param name="Shader" select="$FragmentShader"/>
            <xsl:with-param name="cgcstripOptions">-param -semantic </xsl:with-param>
          </xsl:call-template>
        </xsl:for-each>
      </xsl:for-each>
  </xsl:template>

  <!-- Process Shader Program -->
  <xsl:template name="ShaderProgram">
    <xsl:param name="Profile"/>
    <xsl:param name="Ext"/>
    <xsl:param name="Shader"/>
    <xsl:param name="cgcstripOptions"/>
    <xsl:variable name="Filename" select="../../col:include[1]/@url"/>
    <xsl:variable name="Entry" select="$Shader/col:name"/>

    <xsl:variable name="ShaderName">
      <xsl:call-template name="convert_to_space">
        <xsl:with-param name="text" select="substring-before($Filename, '.cgfx')"/>
        <xsl:with-param name="replace">%20</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>

    <xsl:value-of select="concat('@echo compiling: &quot;%1\', $ShaderName, '_', $Entry, $Ext, '&quot;&#xA;')"/>
    
    <xsl:value-of select="concat('sce-cgc -nobcolor -contalloc -profile ', $Profile)"/>
    <xsl:value-of select="concat(' -entry ', $Entry, ' -o ')"/>
    <xsl:value-of select="concat('&quot;%2/', $ShaderName, '_', $Entry, $Ext, '&quot; ')"/>
    <xsl:value-of select="concat('&quot;%1/', $ShaderName, '.cgfx&quot;&#xA;')"/>

    <xsl:value-of select="concat('sce-cgcstrip ', $cgcstripOptions)"/>
    <xsl:value-of select="concat('&quot;%2/', $ShaderName, '_', $Entry, $Ext, '&quot;&#xA;')"/>
  </xsl:template>


<xsl:template name="convert_to_space">
  <xsl:param name="text"></xsl:param>
  <xsl:param name="replace"></xsl:param>

  <xsl:choose>
    <xsl:when test="contains($text, $replace)">
      <xsl:variable name="rest">
        <xsl:call-template name="convert_to_space">
          <xsl:with-param name="text" select="substring-after($text, $replace)"></xsl:with-param>
          <xsl:with-param name="replace" select="$replace"></xsl:with-param>
        </xsl:call-template>
      </xsl:variable>
      <xsl:value-of select="concat(substring-before($text, $replace), ' ', $rest)"></xsl:value-of>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text"></xsl:value-of>
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>


    <!-- Filter out unprocessed data -->
    <xsl:template match="text()|@*">
    </xsl:template>
  </xsl:stylesheet>